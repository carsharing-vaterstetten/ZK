#include "PinSequencePlayer.h"
#include <HardwareSerial.h>

void SequencePlayer::applySequencePoint(const SequencePoint& point)
{
    point.func();
}

void SequencePlayer::start()
{
    if (sequence.empty()) return;

    sequenceStart = millis();
    sequenceFinished = false;
    currentStateIndex = 0;

    onStarted();
    applySequencePoint(sequence[0]);
}

void SequencePlayer::poll()
{
    if (sequenceFinished || sequence.empty()) return;

    const size_t nextIndex = currentStateIndex + 1;

    if (nextIndex >= sequence.size())
    {
        onCompleted();
        sequenceFinished = true;
        return;
    }

    const SequencePoint& nextSeqPoint = sequence[nextIndex];

    if (deltaT() >= nextSeqPoint.timestamp)
    {
        currentStateIndex = nextIndex;
        applySequencePoint(nextSeqPoint);
    }
}

bool SequencePlayer::completed() const
{
    return sequenceFinished;
}

ulong SequencePlayer::deltaT() const
{
    return millis() - sequenceStart;
}

uint SequencePlayerController::registerPlayer(std::unique_ptr<SequencePlayer> player)
{
    registeredPlayers.emplace_back(std::move(player));
    return registeredPlayers.size() - 1;
}

void SequencePlayerController::poll()
{
    std::vector<uint> toRemove;
    toRemove.reserve(activePlayerIds.size());

    for (const uint id : activePlayerIds)
    {
        auto& player = registeredPlayers[id];

        player->poll();

        if (player->completed())
            toRemove.push_back(id);
    }

    for (const uint id : toRemove)
        onSequenceCompleted(id);
}

void SequencePlayerController::onSequenceCompleted(uint id)
{
    activePlayerIds.erase(id);
}

void SequencePlayerController::makeSequenceActive(uint id)
{
    activePlayerIds.insert(id);
    registeredPlayers[id]->start();
}

bool SequencePlayerController::sequenceCompleted(uint id)
{
    return registeredPlayers[id]->completed();
}
