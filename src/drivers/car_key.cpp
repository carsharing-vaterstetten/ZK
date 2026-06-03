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

void CarKeyDriver::playOpenSequence()
{
    if (!openSequencePlayer.has_value())
    {
        const auto loadedSeq = keySequenceManager.getOpenSequence();
        if (loadedSeq == nullptr) return;
        openSequencePlayer.emplace(*loadedSeq);
    }

    openSequencePlayer->play();
}

void CarKeyDriver::playCloseSequence()
{
    if (!closeSequencePlayer.has_value())
    {
        const auto loadedSeq = keySequenceManager.getCloseSequence();
        if (loadedSeq == nullptr) return;
        closeSequencePlayer.emplace(*loadedSeq);
    }

    closeSequencePlayer->play();
}
