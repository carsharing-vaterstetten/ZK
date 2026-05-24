#pragma once

#include "config/hw_config.h"

class CarKeyDriver
{
public:
    explicit CarKeyDriver(const BoardConfig& board) : board(board) {}

    void begin() const;
    void open() const;
    void close() const;

private:
    const BoardConfig& board;

    static constexpr uint32_t kPulseMs     = 200;
    static constexpr uint32_t kPowerUpMs   = 500;
    static constexpr uint32_t kPowerDownMs = 300;

    void powerOn() const;
    void powerOff() const;
    static void pulse(uint8_t pin);
    void poweredPulse(uint8_t pin) const;
};
