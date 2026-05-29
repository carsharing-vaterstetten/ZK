#pragma once

#include "abstract/PinSequencePlayer.h"
#include "shared/KeySequenceManager.h"


struct SequencePoint;

class CarKeyDriver
{
public:
    CarKeyDriver(uint8_t openKeyPin, uint8_t closeKeyPin, uint8_t keyPowerPin,
                 bool hasPowerKey, const KeySequenceManager& keySequenceManager);

    void begin() const;

    void startOpenSequence();
    [[nodiscard]] bool openSequenceCompleted() const;

    void startCloseSequence();
    [[nodiscard]] bool closeSequenceCompleted() const;
    void pollOpen();
    void pollClose();

protected:
    uint8_t openKeyPin, closeKeyPin, keyPowerPin;
    bool hasPowerKey;

    const KeySequenceManager& keySequenceManager;

    SequencePlayer openSequencePlayer{{}}, closeSequencePlayer{{}};
};
