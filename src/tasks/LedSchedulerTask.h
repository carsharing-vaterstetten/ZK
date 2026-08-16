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
#include "hal/LED.h"

/// Relative importance of a queued LED command. Higher value == more important.
/// A command can only preempt whatever is currently on the strip if its priority
/// is strictly higher than the active command's priority — equal priority never
/// preempts, it just waits its turn (FIFO).
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

    // Delegates to the underlying sequence's own flag rather than storing a
    // second copy — a flash sequence built as non-interruptable should never
    // be treated as interruptable regardless of how it's queued.
    [[nodiscard]] bool isInterruptable() const { return sequence.isInterruptable(); }

private:
    LedPriority priority;
    StatefulSequencePlayer sequence;
    uint id;

    // Optional extra data
    std::shared_ptr<ProgressState> progressState;
};


class LedSchedulerTask : public SystemThread
{
public:
    explicit LedSchedulerTask(StatefulLed& statusLed) : SystemThread(SystemThreadId::LedScheduler, "LEDSCHED", 4096,
                                                                     ThreadPriority::LedScheduler, 0),
                                                        statusLed(statusLed)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

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

    /// Must be called with mtx held. One scheduling decision: retires a finished active command, preempts
    /// it in favor of a higher priority pending command if it's interruptable,
    /// and promotes a pending command into the active slot if nothing is
    /// currently active and the transition gap has elapsed.
    void schedule(TickType_t& nextSequenceTime);

    void updateLed(TickType_t& nextSequenceTime);

    std::atomic<bool> m_running = true;
    std::atomic<uint> m_nextId = 1;

    // Commands waiting for their first turn, OR paused mid-sequence after
    // being preempted by something higher priority. Either way they resume
    // automatically — the underlying StatefulSequencePlayer's idx just picks
    // back up where it stopped — once they're the best candidate again.
    std::map<uint, LedCmd> pendingCommands{};
    std::optional<LedCmd> activeCommand;

    // Earliest tick at which a new command may be promoted into the active
    // slot. Bumped forward by sequenceTransitionDelay every time a command
    // is retired or preempted, so there's always a clean gap between one
    // command ending and the next one lighting up.
    TickType_t transitionReadyAt = 0;

    // Guards pendingCommands, activeCommand and transitionReadyAt together —
    // they change atomically as one scheduling decision.
    std::mutex mtx;

    StatefulLed& statusLed;

    static constexpr TickType_t sequenceTransitionDelay = pdMS_TO_TICKS(100);

    /// Fallback wakeup while the strip is idle. queueCommand() notifies the task
    /// directly, so this only bounds how long shutdown takes to be noticed.
    static constexpr TickType_t idleWait = pdMS_TO_TICKS(1000);
};