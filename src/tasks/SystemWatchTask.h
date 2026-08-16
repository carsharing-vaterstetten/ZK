#pragma once
#include <atomic>

#include "system/SystemThread.h"
#include "system/SystemManager.h"

class SystemWatchTask : public SystemThread
{
public:
    SystemWatchTask() : SystemThread(SystemThreadId::SystemWatchTask, "SWT", 4096, ThreadPriority::SystemWatchTask, 0)
    {
        SystemManager::RegisterThread(this);
    }

protected:
    void setup() override;
    void run() override;

private:
    static void report();

    constexpr static TickType_t reportingFrequency = pdMS_TO_TICKS(5 * 60 * 1000);
};
