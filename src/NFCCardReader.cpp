#include "NFCCardReader.h"

#include "Config.h"
#include "Globals.h"
#include "HardwareManager.h"

PN532* NFCCardReader::nfc = nullptr;
PN532_SPI* NFCCardReader::pn532spi = nullptr;

bool NFCCardReader::init()
{
    fileLog.infoln("Connecting to NFC board...");

    HardwareManager::ensureNFCSPIInitialized();

    pn532spi = new PN532_SPI(*HardwareManager::nfcSpi, NFC_SS);
    nfc = new PN532(*pn532spi);
    nfc->begin();

    const uint32_t versionData = nfc->getFirmwareVersion();

    if (!versionData)
    {
        fileLog.criticalln("Failed to connect to NFC board. No RFID scanning possible");
        return false;
    }

    const String chipName = "PN5" + String(versionData >> 24 & 0xFF, HEX);
    const String firmwareVersion = "Firmware version " + String(versionData >> 16 & 0xFF) + "." + String(
        versionData >> 8 & 0xFF);

    nfc->SAMConfig();

    fileLog.infoln("NFC board connected successfully");
    fileLog.infoln("NFC chip version data: " + chipName + " " + firmwareVersion);

    return true;
}

uint32_t NFCCardReader::readTag()
{
    uint8_t uid[7] = {};
    uint8_t uidLength;

    const bool success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 200);

    if (!success || uidLength < 4) return 0;

    return static_cast<uint32_t>(uid[0]) << 24 |
        static_cast<uint32_t>(uid[1]) << 16 |
        static_cast<uint32_t>(uid[2]) << 8 |
        static_cast<uint32_t>(uid[3]);
}
