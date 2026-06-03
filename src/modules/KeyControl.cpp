#include "KeyControl.h"

void KeyControl::lock() const
{
    driver.playOpenSequence();
}

void KeyControl::unlock() const
{
    driver.playCloseSequence();
}
