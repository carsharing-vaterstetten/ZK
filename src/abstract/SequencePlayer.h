#pragma once
#include <functional>
#include <vector>
#include <sys/types.h>
#include <Arduino.h>
#include <queue>
#include <freertos/FreeRTOS.h>

#include "shared/Globals.h"

struct SequencePoint
{
    ulong timestamp;
    std::function<void()> func;
};

enum SequenceEndStyle
{
    ClearOrMoveToNextSequenceAfterDelay
};

class SequencePlayer
{
public:
    explicit SequencePlayer(std::vector<SequencePoint> sequence) : sequence(std::move(sequence)) {}

    void play() const
    {
        if (sequence.empty()) return;

        TickType_t previousWakeTime = xTaskGetTickCount();

        for (size_t i = 0; i < sequence.size(); ++i)
        {
            applySequencePoint(sequence[i]);

            if (i == sequence.size() - 1) break;

            const TickType_t idealDelay = pdMS_TO_TICKS(sequence[i + 1].timestamp - sequence[i].timestamp);

            vTaskDelayUntil(&previousWakeTime, idealDelay);
        }
    }

private:
    const std::vector<SequencePoint> sequence;

    static void applySequencePoint(const SequencePoint& point)
    {
        point.func();
    }
};

class AsyncSequencePlayer
{
public:
    explicit AsyncSequencePlayer(std::vector<SequencePoint> sequence)
        : sequence(std::move(sequence)), currentIndex(0), nextWakeTick(0), isPlaying(false) {}

    void start()
    {
        if (sequence.empty()) return;

        applySequencePoint(sequence[0]);

        currentIndex = 1;
        isPlaying = (sequence.size() > 1);

        if (isPlaying)
            nextWakeTick = xTaskGetTickCount() + pdMS_TO_TICKS(sequence[1].timestamp - sequence[0].timestamp);
    }

    void update()
    {
        if (!isPlaying) return;

        const TickType_t currentTick = xTaskGetTickCount();

        while (isPlaying && (static_cast<long>(currentTick - nextWakeTick) >= 0))
        {
            applySequencePoint(sequence[currentIndex]);
            currentIndex++;

            if (currentIndex < sequence.size())
            {
                const TickType_t idealDelay = pdMS_TO_TICKS(
                    sequence[currentIndex].timestamp - sequence[currentIndex - 1].timestamp);
                nextWakeTick += idealDelay;
            }
            else
                isPlaying = false;
        }
    }

    [[nodiscard]] bool running() const
    {
        return isPlaying;
    }

private:
    const std::vector<SequencePoint> sequence;
    size_t currentIndex;
    TickType_t nextWakeTick;
    bool isPlaying;

    static void applySequencePoint(const SequencePoint& point)
    {
        point.func();
    }
};

class StatefulSequencePlayer
{
public:
    explicit StatefulSequencePlayer(
        std::vector<SequencePoint> sequence,
        std::function<void()> endState,
        TickType_t endStateDelay,
        const bool interruptable,
        const bool repeat = false,
        const TickType_t repeatDelay = 0
    )
        : sequence(std::move(sequence)), repeat(repeat), repeatDelay(repeatDelay), interruptable(interruptable), endStateDelay(endStateDelay), endState(std::move(endState))
    {
    }

    bool moveToNextState(TickType_t& timeToNextState)
    {
        if (completed)
            return false;

        if (endStateReached)
        {
            endState();
            completed = true;
            return false;
        }

        if (sequence.empty())
        {
            completed = true;
            return false;
        }

        if (idx >= sequence.size())
        {
            completed = true;
            return false;
        }

        const SequencePoint& p = sequence[idx];
        applySequencePoint(p);

        if (idx >= sequence.size() - 1)
        {
            if (repeat)
            {
                idx = 0;
                timeToNextState = repeatDelay;
                return true;
            }

            endStateReached = true;
            timeToNextState = endStateDelay;
            return true;
        }

        const SequencePoint& next_point = sequence[idx + 1];
        timeToNextState = pdMS_TO_TICKS(next_point.timestamp - p.timestamp);
        idx++;

        return true;
    }

    [[nodiscard]] bool isCompleted() const
    {
        return completed;
    }

    [[nodiscard]] bool isInterruptable() const
    {
        return interruptable;
    }

    void stopRepeating()
    {
        repeat = false;
    }

private:
    int idx = 0;
    std::vector<SequencePoint> sequence;
    std::function<void()> endState;
    TickType_t endStateDelay;
    bool endStateReached = false;
    bool repeat;
    bool completed = false;
    bool interruptable;
    TickType_t repeatDelay;

    static void applySequencePoint(const SequencePoint& point)
    {
        point.func();
    }
};
