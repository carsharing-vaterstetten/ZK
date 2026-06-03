#pragma once
#include <functional>
#include <vector>
#include <sys/types.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

struct SequencePoint
{
    ulong timestamp;
    std::function<void()> func;
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
