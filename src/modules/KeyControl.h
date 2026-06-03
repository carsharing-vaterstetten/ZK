#pragma once

#include "drivers/car_key.h"

class KeyControl
{
public:
    explicit KeyControl(CarKeyDriver& driver) : driver(driver) {}

    void lock() const;
    void unlock() const;

protected:
    CarKeyDriver& driver;
};
