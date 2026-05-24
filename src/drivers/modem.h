#pragma once

#include "config/hw_config.h"

class ModemDriver
{
public:
    explicit ModemDriver(const BoardConfig& board) : board(board) {}

    void begin() const;
    void providePower() const;
    void cutPower() const;
    void turnOn() const;
    void turnOff() const;
    void wakeup() const;
    void sleep() const;

protected:
    const BoardConfig& board;
};
