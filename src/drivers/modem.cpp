#include "modem.h"

#include <Arduino.h>

void ModemDriver::begin() const
{
    pinMode(board.modemPowerOn, OUTPUT);
    pinMode(board.modemPwrKey, OUTPUT);
    pinMode(board.modemDtr, OUTPUT);
}

void ModemDriver::providePower() const
{
    digitalWrite(board.modemPowerOn, HIGH);
}

void ModemDriver::cutPower() const
{
    digitalWrite(board.modemPowerOn, LOW);
}

void ModemDriver::turnOn() const
{
    digitalWrite(board.modemPwrKey, LOW);
    delay(100);

    digitalWrite(board.modemPwrKey, HIGH);
    delay(board.modemPwrKeyPulseWidthMs);
    digitalWrite(board.modemPwrKey, LOW);
}

void ModemDriver::turnOff() const
{
    digitalWrite(board.modemPwrKey, LOW);
    delay(100);

    digitalWrite(board.modemPwrKey, HIGH);
    delay(board.modemPowerOffPulseWidthMs);
    digitalWrite(board.modemPwrKey, LOW);
}

void ModemDriver::wakeup() const
{
    digitalWrite(board.modemDtr, LOW);
}
void ModemDriver::sleep() const
{
    digitalWrite(board.modemDtr, HIGH);
}
