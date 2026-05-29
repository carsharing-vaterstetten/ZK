#pragma once
#include <cstdint>

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
    CardReaderService,
    KeyControlService,
    LedService,
    ModemService,
    GPSTask,

    Count // Must be at bottom to track number of registered tasks
};
