#include "hal/ModemHardware.h"

#include <Arduino.h>

void ModemHardwareDriver::begin() const
{
    pinMode(board.modemPowerOn, OUTPUT);
    pinMode(board.modemPwrKey, OUTPUT);
    pinMode(board.modemDtr, OUTPUT);
}

void ModemHardwareDriver::providePower() const
{
    // give power to VBAT (on SIM7000)
    digitalWrite(board.modemPowerOn, HIGH);
}

void ModemHardwareDriver::cutPower() const
{
    digitalWrite(board.modemPowerOn, LOW);
}

void ModemHardwareDriver::powerOn() const
{
    // pwr key is active low on modem, but pcie board connects it via npn, so its active high here
    digitalWrite(board.modemPwrKey, HIGH);
    delay(board.modemPwrKeyPulseWidthMs);
    digitalWrite(board.modemPwrKey, LOW);
}

void ModemHardwareDriver::wakeup() const
{
    digitalWrite(board.modemDtr, LOW);
}
void ModemHardwareDriver::sleep() const
{
    digitalWrite(board.modemDtr, HIGH);
}
