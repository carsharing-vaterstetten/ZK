#pragma once

#include <Arduino.h>
#include <Adafruit_PN532.h>

enum class ScanStatus {
    NoCard,     // No tag detected
    NewCard,    // Valid tag detected and accepted
    Duplicate   // Tag detected but ignored
};

struct ScanResult {
    ScanStatus status;
    uint32_t uid; // Only valid if status is NewCard or Duplicate
};

class NFCCardReader
{
    Adafruit_PN532* _nfc = nullptr;
    std::optional<uint32_t> _lastUid = std::nullopt;
    unsigned long _lastSeenTime = 0;
    const unsigned long _cooldownMs;

    // Helper to handle hardware reading
    [[nodiscard]] std::optional<uint32_t> readRawTag() const;

public:
    explicit NFCCardReader(const unsigned long cooldownMs = 7000) : _cooldownMs(cooldownMs)
    {
    }

    bool init(SPIClass& spi, uint8_t cs);

    [[nodiscard]] ScanResult scan();
};

inline NFCCardReader cardReader{};
