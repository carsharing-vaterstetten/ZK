#pragma once

#include <Arduino.h>
#include <Adafruit_PN532.h>

class NFCCardReader
{
    Adafruit_PN532* nfc = nullptr;

public:
    bool init(SPIClass& spi, uint8_t cs) ;
    bool readTag(uint32_t &uid) const;
};

inline NFCCardReader cardReader{};
