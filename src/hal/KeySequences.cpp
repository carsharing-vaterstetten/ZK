#include "hal/KeySequences.h"

#include <Arduino.h>

KeySequences::KeySequences() {}

void KeySequences::loadSequenceInRAM(const BoardConfig& board)
{
    std::lock_guard lock(mtx);

    static constexpr uint32_t kPulseMs = 200;
    static constexpr uint32_t kPowerUpMs = 100;
    static constexpr uint32_t kPowerDownMs = 200;

    // Extract pin numbers upfront — capture these by value in lambdas
    const uint8_t pinOpen = board.keyOpen;
    const uint8_t pinClose = board.keyClose;

    if (board.hasKeyPower)
    {
        const uint8_t pinPower = board.keyPower.value();

        openSequence = std::make_shared<std::vector<SequencePoint>>(std::vector<SequencePoint>{
            {0, [pinPower] { digitalWrite(pinPower, HIGH); }},
            {kPowerUpMs, [pinOpen] { digitalWrite(pinOpen, HIGH); }},
            {kPowerUpMs + kPulseMs, [pinOpen] { digitalWrite(pinOpen, LOW); }},
            {kPowerUpMs + kPulseMs + kPowerDownMs, [pinPower] { digitalWrite(pinPower, LOW); }},
        });

        closeSequence = std::make_shared<std::vector<SequencePoint>>(std::vector<SequencePoint>{
            {0, [pinPower] { digitalWrite(pinPower, HIGH); }},
            {kPowerUpMs, [pinClose] { digitalWrite(pinClose, HIGH); }},
            {kPowerUpMs + kPulseMs, [pinClose] { digitalWrite(pinClose, LOW); }},
            {kPowerUpMs + kPulseMs + kPowerDownMs, [pinPower] { digitalWrite(pinPower, LOW); }},
        });
    }
    else
    {
        openSequence = std::make_shared<std::vector<SequencePoint>>(std::vector<SequencePoint>{
            {0, [pinOpen] { digitalWrite(pinOpen, HIGH); }},
            {kPulseMs, [pinOpen] { digitalWrite(pinOpen, LOW); }},
        });

        closeSequence = std::make_shared<std::vector<SequencePoint>>(std::vector<SequencePoint>{
            {0, [pinClose] { digitalWrite(pinClose, HIGH); }},
            {kPulseMs, [pinClose] { digitalWrite(pinClose, LOW); }},
        });
    }
}

std::shared_ptr<const std::vector<SequencePoint>> KeySequences::getOpenSequence() const
{
    std::lock_guard lock(mtx);
    return openSequence;
}

std::shared_ptr<const std::vector<SequencePoint>> KeySequences::getCloseSequence() const
{
    std::lock_guard lock(mtx);
    return closeSequence;
}
