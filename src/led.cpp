// led.cpp
#include "LED.h"

#define SerialMon Serial

LED::LED() : led_strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800) {}

void LED::init()
{
    led_strip.begin();
    led_strip.show();
}

// Funktion zeigt die Farben
// für alle LEDs an.
void LED::setStaticColor(String color)
{
    uint32_t colorValue = 0;
    if (color == "blue")
    {
        colorValue = led_strip.Color(0, 0, 255);
    }
    else if (color == "green")
    {
        colorValue = led_strip.Color(0, 255, 0);
    }
    else if (color == "red")
    {
        colorValue = led_strip.Color(255, 0, 0);
    }
    else if (color == "yellow")
    {
        colorValue = led_strip.Color(255, 255, 0);
    }
    else if (color == "orange")
    {
        colorValue = led_strip.Color(255, 165, 0);
    }
    else if (color == "purple")
    {
        colorValue = led_strip.Color(128, 0, 128);
    }
    else if (color == "white")
    {
        colorValue = led_strip.Color(255, 255, 255);
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
        led_strip.setPixelColor(i, colorValue);
    }
    led_strip.show();
}

// Funktion schaltet alle LEDs aus.
void LED::clear()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        led_strip.setPixelColor(i, led_strip.Color(0, 0, 0));
    }
    led_strip.show();
}