#pragma once

#include "abstract/SequencePlayer.h"
#include "shared/KeySequenceManager.h"


struct SequencePoint;

class CarKeyDriver
{
public:
    CarKeyDriver(uint8_t openKeyPin, uint8_t closeKeyPin, uint8_t keyPowerPin,
                 bool hasPowerKey, const KeySequenceManager& keySequenceManager);

    void begin() const;

    void playOpenSequence();
    void playCloseSequence();

protected:
    uint8_t openKeyPin, closeKeyPin, keyPowerPin;
    bool hasPowerKey;

    const KeySequenceManager& keySequenceManager;

    static void playSequence(const std::shared_ptr<const std::vector<SequencePoint>>& sequence);
};
