#pragma once

#include "util/SequencePlayer.h"
#include "hal/KeySequences.h"


struct SequencePoint;

class CarKey
{
public:
    CarKey(uint8_t openKeyPin, uint8_t closeKeyPin, uint8_t keyPowerPin,
                 bool hasPowerKey, const KeySequences& keySequences);

    void begin() const;

    void playOpenSequence();
    void playCloseSequence();

protected:
    uint8_t openKeyPin, closeKeyPin, keyPowerPin;
    bool hasPowerKey;

    const KeySequences& keySequences;

    static void playSequence(const std::shared_ptr<const std::vector<SequencePoint>>& sequence);
};
