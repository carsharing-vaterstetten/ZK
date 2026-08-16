#pragma once
#include <memory>
#include <mutex>

#include "util/SequencePlayer.h"
#include "config/hw_config.h"

class KeySequences
{
public:
    KeySequences();
    void loadSequenceInRAM(const BoardConfig& board);
    std::shared_ptr<const std::vector<SequencePoint>> getOpenSequence() const;
    std::shared_ptr<const std::vector<SequencePoint>> getCloseSequence() const;

protected:
    std::shared_ptr<const std::vector<SequencePoint>> openSequence, closeSequence;

    mutable std::mutex mtx;
};
