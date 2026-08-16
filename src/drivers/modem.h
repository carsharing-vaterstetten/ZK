#pragma once

#include "config/hw_config.h"

class ModemHardwareDriver
{
public:
    explicit ModemHardwareDriver(const BoardConfig& board) : board(board) {}

    void begin() const;
    void providePower() const;
    void cutPower() const;
    void powerOn() const;
    void wakeup() const;
    void sleep() const;

protected:
    const BoardConfig& board;
};
