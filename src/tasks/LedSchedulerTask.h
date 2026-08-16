#pragma once

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>

#include "system/SystemThread.h"
#include "system/SystemManager.h"
#include "hal/Led.h"

/// A command preempts the active one only if its priority is strictly higher.
/// Equal priority waits its turn, oldest first.
enum class LedPriority : uint8_t
{
    Background = 0, // ambient / idle indications
    Normal     = 10, // regular status indications (progress bars, waiting states)
    High       = 20, // direct feedback for a user action (lock/unlock, card result)
    Critical   = 30, // errors / safety relevant, should always be seen
};

class LedCmd
{
public:
    LedCmd(uint id, StatefulSequencePlayer sequence, LedPriority priority = LedPriority::Normal,
           std::shared_ptr<ProgressState> progressState = std::make_shared<ProgressState>())
        : priority(priority),
          sequence(std::move(sequence)),
          id(id),
          progressState(std::move(progressState))
    {
    }

    void markCompleted() { sequence.stopRepeating(); }

    [[nodiscard]] bool completed() const
    {
        return sequence.isCompleted();
    }

    StatefulSequencePlayer* getSequence() { return &sequence; }

    [[nodiscard]] std::shared_ptr<ProgressState> getProgressState() const { return progressState; }

    void setProgressState(const ProgressState& state) { *progressState = state; }

    [[nodiscard]] uint getId() const { return id; }
    [[nodiscard]] LedPriority getPriority() const { return priority; }

    // Delegates rather than storing a second copy, so a sequence built as
    // non-interruptable stays that way however it is queued.
    [[nodiscard]] bool isInterruptable() const { return sequence.isInterruptable(); }

private:
    LedPriority priority;
    StatefulSequencePlayer sequence;
    uint id;

    std::shared_ptr<ProgressState> progressState;
};

class LedSchedulerTask : public SystemThread
{
public:
    explicit LedSchedulerTask(Led& statusLed) : SystemThread(SystemThreadId::LedScheduler, "LEDSCHED", 4096,
                                                                     ThreadPriority::LedScheduler, 0),
                                                        statusLed(statusLed)
    {
        SystemManager::RegisterThread(this);
    }

    void markCommandAsCompleted(uint commandId);
    void updateProgressOfCommand(uint commandId, ProgressState state);

    [[nodiscard]] uint queueProgressIndicator(LedPriority priority = LedPriority::Normal)
    {
        auto state = std::make_shared<ProgressState>();
        LedCmd cmd{generateUniqueCommandId(), statusLed.progressIndicator(state), priority, state};
        return queueCommand(std::move(cmd));
    }

    [[nodiscard]] uint queueLoadingCircle(uint32_t color, LedPriority priority = LedPriority::Normal)
    {
        LedCmd cmd{generateUniqueCommandId(), statusLed.loadingCircle(color), priority};
        return queueCommand(std::move(cmd));
    }

    uint queueUnlockFlash(LedPriority priority = LedPriority::High)
    {
        LedCmd cmd{generateUniqueCommandId(), statusLed.unlockFlash(), priority};
        return queueCommand(std::move(cmd));
    }

    uint queueLockFlash(LedPriority priority = LedPriority::High)
    {
        LedCmd cmd{generateUniqueCommandId(), statusLed.lockFlash(), priority};
        return queueCommand(std::move(cmd));
    }

    uint queueCardDeclinedFlash(LedPriority priority = LedPriority::High)
    {
        LedCmd cmd{generateUniqueCommandId(), statusLed.cardDeclinedFlash(), priority};
        return queueCommand(std::move(cmd));
    }

    uint generateUniqueCommandId()
    {
        return m_nextId.fetch_add(1, std::memory_order_relaxed);
    }

protected:
    void setup() override;
    void run() override;

private:
    uint queueCommand(LedCmd cmd);

    [[nodiscard]] bool nothingToDo();

    /// Must be called with mtx held. Returns the id of the highest-priority
    /// pending command, preferring the oldest one (lowest id) on ties.
    [[nodiscard]] std::optional<uint> pickHighestPriorityPendingId() const;

    /// One scheduling decision: retire, preempt, promote. Call with mtx held.
    void schedule(TickType_t& nextSequenceTime);

    void updateLed(TickType_t& nextSequenceTime);

    std::atomic<uint> m_nextId = 1;

    // Waiting for a first turn, or paused mid-sequence after preemption. Either
    // way they resume where they stopped once they are the best candidate again.
    std::map<uint, LedCmd> pendingCommands{};
    std::optional<LedCmd> activeCommand;

    // Earliest tick a command may be promoted, giving a dark gap between one
    // command ending and the next lighting up.
    TickType_t transitionReadyAt = 0;

    // Guards pendingCommands, activeCommand and transitionReadyAt together.
    std::mutex mtx;

    Led& statusLed;

    static constexpr TickType_t sequenceTransitionDelay = pdMS_TO_TICKS(100);

    /// Fallback wakeup while the strip is idle. queueCommand() notifies the task
    /// directly, so this only bounds how long shutdown takes to be noticed.
    static constexpr TickType_t idleWait = pdMS_TO_TICKS(1000);
};