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

void CarKeyDriver::playSequence(const std::shared_ptr<const std::vector<SequencePoint>>& sequence)
{
    if (sequence == nullptr) return;

    const SequencePlayer player{*sequence};
    player.play();
}

void CarKeyDriver::playOpenSequence()
{
    playSequence(keySequenceManager.getOpenSequence());
}

void CarKeyDriver::playCloseSequence()
{
    playSequence(keySequenceManager.getCloseSequence());
}
