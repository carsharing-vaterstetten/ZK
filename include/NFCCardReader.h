#pragma once

#include <Arduino.h>

#include "../lib/PN532/PN532.h"
#include "../lib/PN532/PN532_SPI.h"

class NFCCardReader
{
    static PN532* nfc;
    static PN532_SPI* pn532spi;

public:
    NFCCardReader() = delete;

    static bool init();
    static uint32_t readTag();
};
