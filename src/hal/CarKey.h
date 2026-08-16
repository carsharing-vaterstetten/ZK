#pragma once

#include "util/SequencePlayer.h"
#include "hal/KeySequences.h"

/// Drives the car's key fob. lock()/unlock() block for the duration of the pin
/// sequence, so they are only ever called from KeyControlTask.
class CarKey
{
public:
    CarKey(uint8_t openKeyPin, uint8_t closeKeyPin, uint8_t keyPowerPin,
           bool hasPowerKey, const KeySequences& keySequences);

    void begin() const;

    void lock();
    void unlock();

protected:
    uint8_t openKeyPin, closeKeyPin, keyPowerPin;
    bool hasPowerKey;

    const KeySequences& keySequences;

    /// Built per press rather than cached: caching pinned the first sequence for
    /// the lifetime of the device, so a later loadSequenceInRAM had no effect.
    static void playSequence(const std::shared_ptr<const std::vector<SequencePoint>>& sequence);
};
