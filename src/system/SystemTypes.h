#pragma once
#include <cstdint>

#define pdMS_TO_TICKS_LONG(xTimeInMs) (TickType_t)(((uint64_t)(xTimeInMs) * configTICK_RATE_HZ) / 1000U)


enum class SystemCommand
{
    None,
    PrepareForHotRestart,
    EnterLowPower,
    ResumeNormalOperation
};

enum class SystemThreadId : uint8_t
{
    RestartTask,
    AccessControlTask,
    CardReaderTask,
    KeyControlTask,
    LedScheduler,
    ModemTask,
    GpsTask,
    StartupTask,
    SystemWatchTask,

    Count // Must be at bottom to track number of registered tasks
};

enum class ThreadPriority : UBaseType_t
{
    // Unimportant
    RestartTask = 1,
    GpsTask = 1,
    StartupTask = 1,
    SystemWatchTask = 1,

    // User experience
    ModemTask = 2,
    AccessControlTask = 3,
    CardReaderTask = 3,
    LedScheduler = 3,

    // Critical timing
    KeyControlTask = 4,
};
