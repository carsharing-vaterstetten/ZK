#include "KeyControl.h"

void KeyControl::lock()
{
    driver.playCloseSequence();
}

void KeyControl::unlock()
{
    driver.playOpenSequence();
}
