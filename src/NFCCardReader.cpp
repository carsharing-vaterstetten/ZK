#include "NFCCardReader.h"

#include "Config.h"
#include "Globals.h"
#include "HardwareManager.h"

Adafruit_PN532* NFCCardReader::nfc = nullptr;

bool NFCCardReader::init()
{
    fileLog.infoln("Connecting to NFC board...");

    HardwareManager::ensureNFCSPIInitialized();

    nfc = new Adafruit_PN532(NFC_SS, HardwareManager::nfcSpi);
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

bool NFCCardReader::readTag(uint32_t& uid)
{
    uint8_t uidArr[7] = {};
    uint8_t uidLength;

    const bool success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidArr, &uidLength, 200);

    if (!success || uidLength != 4) return false;

    uid = static_cast<uint32_t>(uidArr[0]) << 24 |
        static_cast<uint32_t>(uidArr[1]) << 16 |
        static_cast<uint32_t>(uidArr[2]) << 8 |
        static_cast<uint32_t>(uidArr[3]);

    return true;
}
