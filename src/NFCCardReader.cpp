#include "NFCCardReader.h"

#include <SPI.h>

#include "Config.h"
#include "Globals.h"

PN532* NFCCardReader::nfc = nullptr;
PN532_SPI* NFCCardReader::pn532spi = nullptr;
SPIClass* NFCCardReader::spi = nullptr;

bool NFCCardReader::init()
{
    spi = new SPIClass(HSPI);
    spi->begin(NFC_SCLK, NFC_MISO, NFC_MOSI);
    pn532spi = new PN532_SPI(*spi, NFC_SS);
    nfc = new PN532(*pn532spi);
    nfc->begin();

    fileLog.infoln("NFC board initialized");

    const uint32_t versionData = nfc->getFirmwareVersion();

    if (!versionData)
    {
        fileLog.criticalln("Could not get NFC chip version data");
        return false;
    }

    const String chipName = "PN5" + String(versionData >> 24 & 0xFF, HEX);
    const String firmwareVersion = "Firmware version " + String(versionData >> 16 & 0xFF) + "." + String(
        versionData >> 8 & 0xFF);

    fileLog.infoln("Got NFC chip version data: " + chipName + " " + firmwareVersion);

    nfc->SAMConfig();

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
