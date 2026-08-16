#pragma once

#include "hal/CarKey.h"

class KeyControl
{
public:
    explicit KeyControl(CarKey& driver) : driver(driver) {}

    // Not const: driving the key is a stateful hardware action. Marking these
    // const only worked because constness does not propagate through a reference
    // member, which hid the mutation from every caller up the chain.
    void lock();
    void unlock();

protected:
    CarKey& driver;
};
