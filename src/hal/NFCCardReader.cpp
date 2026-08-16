#include "hal/NFCCardReader.h"

#include "logging/Loggers.h"

bool NFCCardReader::connect() const
{
    fileLog.debugln("Connecting to NFC board...");

    if (!nfc.getFirmwareVersion())
    {
        fileLog.criticalln("Failed to connect to NFC board. No RFID scanning possible");
        return false;
    }

    nfc.SAMConfig();

    fileLog.infoln("NFC board connected successfully");

    return true;
}

std::optional<uint32_t> NFCCardReader::scan() const
{
    uint8_t uidArr[7] = {};
    uint8_t uidLen = 0;

    if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uidArr, &uidLen, 200))
        return std::nullopt;

    if (uidLen != 4) return std::nullopt;

    return static_cast<uint32_t>(uidArr[0]) << 24 |
        static_cast<uint32_t>(uidArr[1]) << 16 |
        static_cast<uint32_t>(uidArr[2]) << 8 |
        static_cast<uint32_t>(uidArr[3]);
}
