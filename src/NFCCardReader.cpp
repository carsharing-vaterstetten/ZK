#include "NFCCardReader.h"

#include "Globals.h"

NFCCardReader::NFCCardReader(SPIClass& spi, const uint8_t cs, const unsigned long cooldownMs) : Adafruit_PN532(cs, &spi),
    _cooldownMs(cooldownMs)
{
}

bool NFCCardReader::begin()
{
    fileLog.debugln("Connecting to NFC board...");

    if (!Adafruit_PN532::begin())
    {
        fileLog.criticalln("Failed to initialize NFC card reader. No RFID scanning possible");
        return false;
    }

    if (!getFirmwareVersion())
    {
        fileLog.criticalln("Failed to connect to NFC board. No RFID scanning possible");
        return false;
    }

    SAMConfig();

    fileLog.infoln("NFC board connected successfully");

    return true;
}

std::optional<uint32_t> NFCCardReader::readRawTag()
{
    uint8_t uidArr[7] = {};
    uint8_t uidLen = 0;

    if (!readPassiveTargetID(PN532_MIFARE_ISO14443A, uidArr, &uidLen, 200))
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
