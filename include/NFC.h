// NFC.h
#ifndef NFC_H
#define NFC_H

#include <Arduino.h>
#include <Adafruit_PN532.h>

class NFC
{
public:
    NFC();
    bool init();
    uint32_t readTag();

private:
    Adafruit_PN532 nfc;
};

#endif
