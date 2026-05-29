#pragma once
#include <esp32-hal-timer.h>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include <sys/types.h>

struct SequencePoint
{
    ulong timestamp;
    std::function<void()> func;
};

static void noop() {}

class SequencePlayer
{
public:
    explicit SequencePlayer(std::vector<SequencePoint> sequence, std::function<void()> onStarted = noop,
                            std::function<void()> onCompleted = noop) : sequence(std::move(sequence)),
                                                                        onStarted(std::move(onStarted)),
                                                                        onCompleted(std::move(onCompleted)) {}

    void start();
    void poll();

    [[nodiscard]] bool completed() const;
    [[nodiscard]] ulong deltaT() const;

protected:
    ulong sequenceStart = 0;
    bool sequenceFinished = true;
    size_t currentStateIndex = 0;
    std::vector<SequencePoint> sequence;
    std::function<void()> onStarted, onCompleted;

    static void applySequencePoint(const SequencePoint& point);
};

class SequencePlayerController
{
public:
    SequencePlayerController() {};

    uint registerPlayer(std::unique_ptr<SequencePlayer> player);
    void makeSequenceActive(uint id);
    bool sequenceCompleted(uint id);
    void poll();

protected:
    std::vector<std::unique_ptr<SequencePlayer>> registeredPlayers{};
    std::set<uint> activePlayerIds{};

    void onSequenceCompleted(uint id);
};
