#pragma once

#include <Arduino.h>
#include <Adafruit_PN532.h>

class NFCCardReader
{
    static Adafruit_PN532 * nfc;

public:
    NFCCardReader() = delete;

    static bool init();
    static bool readTag(uint32_t &uid);
};
