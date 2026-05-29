#include "car_key.h"
#include <Arduino.h>

CarKeyDriver::CarKeyDriver(const uint8_t openKeyPin, const uint8_t closeKeyPin, const uint8_t keyPowerPin,
                           const bool hasPowerKey, const KeySequenceManager& keySequenceManager) :
    openKeyPin(openKeyPin), closeKeyPin(closeKeyPin), keyPowerPin(keyPowerPin), hasPowerKey(hasPowerKey),
    keySequenceManager(keySequenceManager) {}


void CarKeyDriver::begin() const
{
    pinMode(openKeyPin, OUTPUT);
    pinMode(closeKeyPin, OUTPUT);

    if (hasPowerKey)
        pinMode(keyPowerPin, OUTPUT);
}

void CarKeyDriver::startOpenSequence()
{
    openSequencePlayer = SequencePlayer{*keySequenceManager.getOpenSequence()};
    openSequencePlayer.start();
}

bool CarKeyDriver::openSequenceCompleted() const
{
    return openSequencePlayer.completed();
}

void CarKeyDriver::startCloseSequence()
{
    closeSequencePlayer = SequencePlayer{*keySequenceManager.getCloseSequence()};
    closeSequencePlayer.start();
}

bool CarKeyDriver::closeSequenceCompleted() const
{
    return closeSequencePlayer.completed();
}

void CarKeyDriver::pollOpen() // TODO: no polling
{
    openSequencePlayer.poll();
}

void CarKeyDriver::pollClose()
{
    closeSequencePlayer.poll();
}
