#include "NFC.h"

#include "Config.h"
#include "Globals.h"

NFC::NFC() : nfc(NFC_SCLK, NFC_MISO, NFC_MOSI, NFC_SS)
{
}

bool NFC::init()
{
    sleep(1);
    const bool nfcInitSuccess = nfc.begin();
    sleep(1);

    if (!nfcInitSuccess)
    {
        fileLog.errorln("Failed to initialize NFC board");
        return false;
    }

    fileLog.infoln("NFC board successfully initialized");

    const uint32_t versionData = nfc.getFirmwareVersion();

    if (!versionData)
    {
        fileLog.warningln("Could not get NFC chip version data");
        return false;
    }

    const String chipName = "PN5" + String(versionData >> 24 & 0xFF, HEX);
    const String firmwareVersion = "Firmware version " + String(versionData >> 16 & 0xFF) + "." + String(
        versionData >> 8 & 0xFF);

    fileLog.infoln("Got NFC chip version data: " + chipName + " " + firmwareVersion);

    return true;
}

uint32_t NFC::readTag()
{
    uint8_t uid[7] = {};
    uint8_t uidLength;

    const bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

    if (!success || uidLength < 4) return 0;

    return static_cast<uint32_t>(uid[0]) << 24 |
        static_cast<uint32_t>(uid[1]) << 16 |
        static_cast<uint32_t>(uid[2]) << 8 |
        static_cast<uint32_t>(uid[3]);
}
