#include "car_key.h"
#include <Arduino.h>

void CarKeyDriver::begin() const
{
    pinMode(board.keyOpen, OUTPUT);
    pinMode(board.keyClose, OUTPUT);

    if (board.hasKeyPower)
        pinMode(*board.keyPower, OUTPUT);
}

void CarKeyDriver::open() const
{
    poweredPulse(board.keyOpen);
}

void CarKeyDriver::close() const
{
    poweredPulse(board.keyClose);
}

void CarKeyDriver::powerOn() const
{
    if (board.hasKeyPower)
        digitalWrite(*board.keyPower, HIGH);
}

void CarKeyDriver::powerOff() const
{
    if (board.hasKeyPower)
        digitalWrite(*board.keyPower, LOW);
}

void CarKeyDriver::pulse(const uint8_t pin)
{
    digitalWrite(pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(kPulseMs));
    digitalWrite(pin, LOW);
}

void CarKeyDriver::poweredPulse(const uint8_t pin) const
{
    if (board.hasKeyPower)
    {
        powerOn();
        vTaskDelay(pdMS_TO_TICKS(kPowerUpMs));
    }

    pulse(pin);

    if (board.hasKeyPower)
    {
        vTaskDelay(pdMS_TO_TICKS(kPowerDownMs));
        powerOff();
    }
}
