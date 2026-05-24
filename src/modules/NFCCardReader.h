#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <optional>
#include <SPI.h>
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

class NFCCardReader
{
protected:
    std::optional<uint32_t> _lastUid = std::nullopt;
    ulong _lastSeenTime = 0;
    const ulong _cooldownMs;
    Adafruit_PN532& nfc;

    // Helper to handle hardware reading
    [[nodiscard]] std::optional<uint32_t> readRawTag(bool detected) const;

public:
    explicit NFCCardReader(Adafruit_PN532& nfcDriver, ulong cooldownMs = 7000);
    bool begin() const;

    [[nodiscard]] ScanResult scan(bool detected);
    void startPassiveDetect() const;


};
