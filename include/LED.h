#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum class StatusColor
{
    PerformingOTAUpdate,
    Error,
};

enum class Color
{
    Red,
    Orange,
    Yellow,
    Purple,
    Blue,
    Green,
    White,
};


class LED
{
public:
    LED(uint16_t ledCount, int16_t ledPin, neoPixelType type);


    bool init();
    void setColor(Color color);
    void setStatusColor(StatusColor color);
    void clear();

private:
    Adafruit_NeoPixel neo;
    static uint32_t getColorValue(Color color);
    static uint32_t getStatusColorValue(StatusColor color);
};
