#pragma once
#include <atomic>

#include "SystemThread.h"
#include "logic/SystemManager.h"

class SystemWatchTask : public SystemThread
{
public:
    SystemWatchTask() : SystemThread(SystemThreadId::SystemWatchTask, "SWT", 4096, ThreadPriority::SystemWatchTask, 0)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;
    static void log();

    constexpr static TickType_t reportingFrequency = pdMS_TO_TICKS(5 * 60 * 1000);
};
