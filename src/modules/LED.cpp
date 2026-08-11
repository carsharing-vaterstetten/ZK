#include "LED.h"

LED::LED(Adafruit_NeoPixel& ledDriver) : neo(ledDriver) {}

void LED::setColor(const uint32_t hex) const
{
    neo.fill(hex);
    neo.show();
}

void LED::clear() const
{
    neo.clear();
    neo.show();
}
