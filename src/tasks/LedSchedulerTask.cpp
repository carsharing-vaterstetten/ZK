#include "LedSchedulerTask.h"

#include "hal/Led.h"

namespace
{
    // Signed difference, so the check survives the tick counter wrapping.
    bool tickHasPassed(TickType_t now, TickType_t deadline)
    {
        return static_cast<int32_t>(now - deadline) >= 0;
    }
}

uint LedSchedulerTask::queueCommand(LedCmd cmd)
{
    uint id;

    {
        std::lock_guard lock(mtx);
        id = cmd.getId();
        pendingCommands.emplace(id, std::move(cmd));
    }

    // So a queued command lights up without waiting out the idle sleep.
    notifySelf();

    return id;
}

void LedSchedulerTask::markCommandAsCompleted(uint commandId)
{
    std::lock_guard lock(mtx);

    if (activeCommand.has_value() && activeCommand->getId() == commandId)
    {
        activeCommand->markCompleted();
        return;
    }

    if (!pendingCommands.count(commandId)) return;

    pendingCommands.at(commandId).markCompleted();
}

void LedSchedulerTask::updateProgressOfCommand(uint commandId, ProgressState state)
{
    std::lock_guard lock(mtx);

    if (activeCommand.has_value() && activeCommand->getId() == commandId)
    {
        activeCommand->setProgressState(state);
        return;
    }

    if (!pendingCommands.count(commandId)) return;

    pendingCommands.at(commandId).setProgressState(state);
}

void LedSchedulerTask::setup()
{
    statusLed.clear();
}

bool LedSchedulerTask::nothingToDo()
{
    std::lock_guard lock(mtx);
    return pendingCommands.empty() && !activeCommand.has_value();
}

std::optional<uint> LedSchedulerTask::pickHighestPriorityPendingId() const
{
    std::optional<uint> best;
    LedPriority bestPriority{};

    // Ascending id order == oldest first, so the first command to reach a
    // given priority level keeps it on ties (FIFO within a priority level).
    for (const auto& [id, cmd] : pendingCommands)
    {
        if (!best.has_value() || cmd.getPriority() > bestPriority)
        {
            best = id;
            bestPriority = cmd.getPriority();
        }
    }

    return best;
}

void LedSchedulerTask::schedule(TickType_t& nextSequenceTime)
{
    const TickType_t now = xTaskGetTickCount();

    // Drop anything cancelled while it sat pending, so it cannot be promoted.
    for (auto it = pendingCommands.begin(); it != pendingCommands.end();)
    {
        if (it->second.completed())
            it = pendingCommands.erase(it);
        else
            ++it;
    }

    // Retire the active command once its sequence has actually finished.
    if (activeCommand.has_value() && activeCommand->completed())
    {
        activeCommand.reset();
        statusLed.clear();
        transitionReadyAt = now + sequenceTransitionDelay;
    }

    // An interruptable command steps aside for a strictly higher priority one,
    // going back into the pending pool so it can resume where it left off.
    if (activeCommand.has_value() && activeCommand->isInterruptable())
    {
        if (const auto challengerId = pickHighestPriorityPendingId(); challengerId.has_value() &&
            pendingCommands.at(*challengerId).getPriority() > activeCommand->getPriority())
        {
            auto parked = std::move(*activeCommand);
            const uint parkedId = parked.getId();
            pendingCommands.emplace(parkedId, std::move(parked));
            activeCommand.reset();
            statusLed.clear();
            transitionReadyAt = now + sequenceTransitionDelay;
        }
    }

    // Promote, but only once the transition gap has elapsed.
    if (!activeCommand.has_value() && !pendingCommands.empty())
    {
        if (tickHasPassed(now, transitionReadyAt))
        {
            const uint pickedId = *pickHighestPriorityPendingId();
            auto node = pendingCommands.extract(pickedId);
            activeCommand = std::move(node.mapped());
        }
        else
        {
            nextSequenceTime = transitionReadyAt - now;
            return;
        }
    }

    if (!activeCommand.has_value())
        nextSequenceTime = pdMS_TO_TICKS(10);
}

void LedSchedulerTask::updateLed(TickType_t& nextSequenceTime)
{
    // Held across advancing the sequence, not just across scheduling: the player
    // mutated here is the same object markCommandAsCompleted() and
    // updateProgressOfCommand() touch from other tasks.
    std::lock_guard lock(mtx);

    schedule(nextSequenceTime);

    if (!activeCommand.has_value()) return;

    activeCommand->getSequence()->moveToNextState(nextSequenceTime);
}

void LedSchedulerTask::run()
{
    TickType_t nextSequenceTime = 0;

    while (isRunning())
    {
        if (nothingToDo())
        {
            // Sleep until someone queues work rather than polling an empty map.
            ulTaskNotifyTake(pdTRUE, idleWait);
            continue;
        }

        updateLed(nextSequenceTime);

        vTaskDelay(nextSequenceTime);
    }

    statusLed.clear();
}