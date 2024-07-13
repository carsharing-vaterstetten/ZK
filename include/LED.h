// led.h
#ifndef led_H
#define led_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Config.h>

class LED
{
public:
    LED();
    void init();
    void setStaticColor(String color);
    void clear();

private:
    Adafruit_NeoPixel led_strip;
};

#endif
