// NFC.cpp
#include "NFC.h"

#define SerialMon Serial

#ifdef NCF_I2C
NFC::NFC() : nfc(NCF_SDA, NCF_SCL) {}
#else
NFC::NFC() : nfc(NFC_SCLK, NFC_MISO, NFC_MOSI, NFC_SS) {}
#endif

void NFC::init()
{
    sleep(1);
    nfc.begin();
    sleep(1);
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
        SerialMon.print("Didn't find PN53x board");
        while (1)
            ;
    }

    SerialMon.print("Found chip PN5");
    SerialMon.println((versiondata >> 24) & 0xFF, HEX);
    SerialMon.print("Firmware ver. ");
    SerialMon.print((versiondata >> 16) & 0xFF, DEC);
    SerialMon.print('.');
    SerialMon.println((versiondata >> 8) & 0xFF, DEC);
}

String NFC::bytesToHexValue(uint8_t bytes[], int nLen)
{
    String chipValue = "";

    for (int i = 0; i < nLen; i++)
    {
        char OneByte[3];
        sprintf(OneByte, "%02X", bytes[i]);
        chipValue += OneByte;
    }

    return chipValue;
}

String NFC::readTag()
{
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    boolean success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

    if (success)
    {
        return bytesToHexValue(uid, uidLength);
    }
    return "";
}