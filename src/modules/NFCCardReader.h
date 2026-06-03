#pragma once

#include <optional>
#include <Adafruit_PN532.h>

class NFCCardReader
{
public:
    explicit NFCCardReader(Adafruit_PN532& nfcDriver) : nfc(nfcDriver){}

    bool connect() const;

    [[nodiscard]] std::optional<uint32_t> scan() const;

private:
    Adafruit_PN532& nfc;
};
