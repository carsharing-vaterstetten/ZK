#pragma once
#include <cstdint>
#include <optional>
#include <esp32-hal-spi.h>

// ── Board config ──────────────────────────────────────────────────────────────
struct BoardConfig
{
    // Features
    bool hasNFCInterrupt;
    bool hasKeyPower;

    // LED
    uint8_t led;
    uint8_t ledCount;

    // Modem
    uint8_t modemDtr;
    uint8_t modemRx;
    uint8_t modemTx;
    uint8_t modemPowerOn;
    uint8_t modemPwrKey;
    uint32_t modemPwrKeyPulseWidthMs;
    uint32_t modemPowerOffPulseWidthMs;

    // Key
    uint8_t keyOpen;
    uint8_t keyClose;
    std::optional<uint8_t> keyPower;

    // NFC
    uint8_t nfcMosi;
    uint8_t nfcMiso;
    uint8_t nfcClk;
    uint8_t nfcCs;
    uint8_t nfcSpi;
    std::optional<uint8_t> nfcIrq;
};

// ── Board instances ───────────────────────────────────────────────────────────
inline constexpr BoardConfig BOARD_REV2 = {
    .hasNFCInterrupt = false, .hasKeyPower = false,
    .led = 12, .ledCount = 4,
    .modemDtr = 32, .modemRx = 26, .modemTx = 27, .modemPowerOn = 25, .modemPwrKey = 4, .modemPwrKeyPulseWidthMs = 1000, .modemPowerOffPulseWidthMs = 1300,
    .keyOpen = 15, .keyClose = 14, .keyPower = std::nullopt,
    .nfcMosi = 23, .nfcMiso = 19, .nfcClk = 18, .nfcCs = 5, .nfcSpi = HSPI, .nfcIrq = std::nullopt
};

inline constexpr BoardConfig BOARD_REV3 = {
    .hasNFCInterrupt = true, .hasKeyPower = true,
    .led = 12, .ledCount = 4,
    .modemDtr = 32, .modemRx = 26, .modemTx = 27, .modemPowerOn = 25, .modemPwrKey = 4, .modemPwrKeyPulseWidthMs = 1000, .modemPowerOffPulseWidthMs = 1300,
    .keyOpen = 15, .keyClose = 14, .keyPower = 32,
    .nfcMosi = 23, .nfcMiso = 19, .nfcClk = 18, .nfcCs = 5, .nfcSpi = HSPI, .nfcIrq = 13
};

// ── Runtime selection ─────────────────────────────────────────────────────────
constexpr const BoardConfig* getBoard(const uint32_t rev)
{
    if (rev == 2) return &BOARD_REV2;
    if (rev == 3) return &BOARD_REV3;
    return nullptr;
}

template <uint32_t Rev>
constexpr const BoardConfig* activeBoard()
{
    static_assert(Rev != Rev, "Unknown hardware revision");
    return nullptr;
}

template <>
constexpr const BoardConfig* activeBoard<2>() { return &BOARD_REV2; }

template <>
constexpr const BoardConfig* activeBoard<3>() { return &BOARD_REV3; }
