#include "NFCCardReader.h"

#include "Globals.h"

NFCCardReader::NFCCardReader(Adafruit_PN532& nfcDriver, const ulong cooldownMs) : _cooldownMs(cooldownMs),
    nfc(nfcDriver) {}

bool NFCCardReader::begin() const
{
    fileLog.debugln("Connecting to NFC board...");

    if (!nfc.begin())
    {
        fileLog.criticalln("Failed to initialize NFC card reader. No RFID scanning possible");
        return false;
    }

    if (!nfc.getFirmwareVersion())
    {
        fileLog.criticalln("Failed to connect to NFC board. No RFID scanning possible");
        return false;
    }

    nfc.SAMConfig();

    fileLog.infoln("NFC board connected successfully");

    return true;
}

std::optional<uint32_t> NFCCardReader::readRawTag(const bool detected) const
{
    uint8_t uidArr[7] = {};
    uint8_t uidLen = 0;

    if (detected)
    {
        if (!nfc.readDetectedPassiveTargetID(uidArr, &uidLen))
            return std::nullopt;
    }
    else
    {
        if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uidArr, &uidLen, 200))
            return std::nullopt;
    }

    if (uidLen != 4) return std::nullopt;

    return static_cast<uint32_t>(uidArr[0]) << 24 |
        static_cast<uint32_t>(uidArr[1]) << 16 |
        static_cast<uint32_t>(uidArr[2]) << 8 |
        static_cast<uint32_t>(uidArr[3]);
}

ScanResult NFCCardReader::scan(const bool detected)
{
    const std::optional<uint32_t> currentUid = readRawTag(detected);

    if (!currentUid)
    {
        return {ScanStatus::NoCard, 0};
    }

    const ulong now = millis();
    bool isDuplicate = false;

    if (_lastUid.has_value() && *_lastUid == *currentUid && now - _lastSeenTime < _cooldownMs)
        isDuplicate = true;

    // Update state
    _lastUid = currentUid;
    _lastSeenTime = now;

    return {isDuplicate ? ScanStatus::Duplicate : ScanStatus::NewCard, *currentUid};
}

void NFCCardReader::startPassiveDetect() const
{
    nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
}
