// NFC.cpp
#include "NFC.h"

#define SerialMon Serial

NFC::NFC() : nfc(NFC_MISO, NFC_MOSI, NFC_SCLK, NFC_SS) {}

void NFC::init()
{
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
    SerialMon.println("Waiting for a Card ...");
}

char *NFC::bytesToHexValue(uint8_t bytes[], int nLen)
{
    int i = 0;
    char OneByte[3];
    char *chipValue = new char[nLen * 2 + 1];
    chipValue[0] = '\0';

    for (i = 0; i < nLen; i++)
    {
        sprintf(OneByte, "%02X", bytes[i]);
        strcat(chipValue, OneByte);
    }
    return chipValue;
}

char *NFC::readTag()
{
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    boolean success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 30); // 30ms timeout

    if (success)
    {
        return bytesToHexValue(uid, uidLength);
    }
    return "";
}