#include "LedSchedulerTask.h"

#include "modules/LED.h"

namespace
{
    // TickType_t is an unsigned counter that wraps around, so a deadline
    // check has to use a signed difference rather than a plain `now >=
    // deadline` comparison, or it'll misbehave once the tick count wraps.
    bool tickHasPassed(TickType_t now, TickType_t deadline)
    {
        return static_cast<int32_t>(now - deadline) >= 0;
    }
}

void LedSchedulerTask::OnCommand(SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        break;
    case SystemCommand::EnterLowPower:
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

uint LedSchedulerTask::queueCommand(LedCmd cmd)
{
    std::lock_guard lock(mtx);
    const uint id = cmd.getId();
    pendingCommands.emplace(id, std::move(cmd));
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
    std::lock_guard lock(mtx);

    const TickType_t now = xTaskGetTickCount();

    // Drop anything that was cancelled/completed while it sat in the pending
    // pool (e.g. cancelled while paused), so it can never get promoted.
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

    // Preempt: an interruptable active command steps aside for a strictly
    // higher priority pending one. It goes back into the pending pool rather
    // than being dropped, so it resumes right where it left off (same idx
    // inside its StatefulSequencePlayer) once it's the best candidate again.
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

    // Promote the best pending command into the active slot — but only once
    // the post-transition gap has elapsed. This is what puts a clean ~100ms
    // dark gap between one command finishing/being preempted and the next
    // one lighting up, instead of snapping straight from one to the other.
    if (!activeCommand.has_value() && !pendingCommands.empty())
    {
        if (tickHasPassed(now, transitionReadyAt))
        {
            const uint pickedId = *pickHighestPriorityPendingId();
            auto node = pendingCommands.extract(pickedId);
            activeCommand = std::move(node.mapped());
            statusLed.loadSequence(activeCommand->getSequence());
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
    schedule(nextSequenceTime);

    if (!activeCommand.has_value())
        return;

    statusLed.nextState(nextSequenceTime);
}

void LedSchedulerTask::run()
{
    TickType_t nextSequenceTime = 0;

    while (m_running)
    {
        if (nothingToDo())
            nextSequenceTime = pdMS_TO_TICKS(10);
        else
            updateLed(nextSequenceTime);

        vTaskDelay(nextSequenceTime);
    }

    statusLed.stopActiveSequence();
    statusLed.clear();

    SystemManager::ReportReadyForRestart(m_id);
}