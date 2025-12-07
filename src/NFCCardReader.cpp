#include "NFCCardReader.h"

#include "Config.h"

bool NFCCardReader::init(SPIClass& spi, const uint8_t cs)
{
    fileLog.debugln("Connecting to NFC board...");

    delete _nfc;
    _nfc = new Adafruit_PN532{cs, &spi};

    _nfc->begin();

    const uint32_t versionData = _nfc->getFirmwareVersion();

    if (!versionData)
    {
        fileLog.criticalln("Failed to connect to NFC board. No RFID scanning possible");
        return false;
    }

    const String chipName = "PN5" + String(versionData >> 24 & 0xFF, HEX);
    const String firmwareVersion = "Firmware version " + String(versionData >> 16 & 0xFF) + "." + String(
        versionData >> 8 & 0xFF);

    _nfc->SAMConfig();

    fileLog.infoln("NFC board connected successfully");
    fileLog.infoln("NFC chip version data: " + chipName + " " + firmwareVersion);

    return true;
}

std::optional<uint32_t> NFCCardReader::readRawTag() const
{
    if (_nfc == nullptr) return std::nullopt;

    uint8_t uidArr[7] = {};
    uint8_t uidLen = 0;

    if (!_nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidArr, &uidLen, 200))
        return std::nullopt;

    if (uidLen != 4) return std::nullopt;

    return static_cast<uint32_t>(uidArr[0]) << 24 |
        static_cast<uint32_t>(uidArr[1]) << 16 |
        static_cast<uint32_t>(uidArr[2]) << 8 |
        static_cast<uint32_t>(uidArr[3]);
}

ScanResult NFCCardReader::scan()
{
    const std::optional<uint32_t> currentUid = readRawTag();

    if (!currentUid)
    {
        return {ScanStatus::NoCard, 0};
    }

    const unsigned long now = millis();
    bool isDuplicate = false;

    if (_lastUid.has_value() && *_lastUid == *currentUid && (now - _lastSeenTime < _cooldownMs))
        isDuplicate = true;

    // Update state
    _lastUid = currentUid;
    _lastSeenTime = now;

    return {isDuplicate ? ScanStatus::Duplicate : ScanStatus::NewCard, *currentUid};
}
