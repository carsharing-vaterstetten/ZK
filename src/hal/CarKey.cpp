#include "hal/CarKey.h"
#include <Arduino.h>

CarKey::CarKey(const uint8_t openKeyPin, const uint8_t closeKeyPin, const uint8_t keyPowerPin,
                           const bool hasPowerKey, const KeySequences& keySequences) :
    openKeyPin(openKeyPin), closeKeyPin(closeKeyPin), keyPowerPin(keyPowerPin), hasPowerKey(hasPowerKey),
    keySequences(keySequences) {}


void CarKey::begin() const
{
    pinMode(openKeyPin, OUTPUT);
    pinMode(closeKeyPin, OUTPUT);

    if (hasPowerKey)
        pinMode(keyPowerPin, OUTPUT);
}

void CarKey::playSequence(const std::shared_ptr<const std::vector<SequencePoint>>& sequence)
{
    if (sequence == nullptr) return;

    const SequencePlayer player{*sequence};
    player.play();
}

void CarKey::playOpenSequence()
{
    playSequence(keySequences.getOpenSequence());
}

void CarKey::playCloseSequence()
{
    playSequence(keySequences.getCloseSequence());
}
