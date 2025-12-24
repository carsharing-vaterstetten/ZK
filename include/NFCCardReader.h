#pragma once

#include <Arduino.h>
#include <Adafruit_PN532.h>

enum class ScanStatus
{
    NoCard, // No tag detected
    NewCard, // Valid tag detected and accepted
    Duplicate // Tag detected but ignored
};

struct ScanResult
{
    ScanStatus status;
    uint32_t uid; // Only valid if status is NewCard or Duplicate
};

class NFCCardReader : protected Adafruit_PN532
{
    std::optional<uint32_t> _lastUid = std::nullopt;
    unsigned long _lastSeenTime = 0;
    const unsigned long _cooldownMs;

    // Helper to handle hardware reading
    [[nodiscard]] std::optional<uint32_t> readRawTag();

public:
    NFCCardReader(SPIClass& spi, uint8_t cs, unsigned long cooldownMs = 7000);
    bool begin();

    [[nodiscard]] ScanResult scan();
};
