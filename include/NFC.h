// NFC.h
#ifndef NFC_H
#define NFC_H

#include <Arduino.h>
#include <Adafruit_PN532.h>
#include <Config.h>

class NFC
{
public:
    NFC();
    void init();
    char *readTag();
    char *bytesToHexValue(uint8_t bytes[], int nLen);

private:
    Adafruit_PN532 nfc;
};

#endif
