#pragma once
#include <atomic>

#include "system/SystemManager.h"
#include "tasks/ModemService.h"

class RestartTask : public SystemThread
{
public:
    explicit RestartTask(const ulong targetRestartTimeMs, ModemService& modem)
        : SystemThread(SystemThreadId::RestartTask, "RSTRT", 4096, ThreadPriority::RestartTask, 0), targetRestartTimeMs(targetRestartTimeMs),
          modem(modem)
    {
        SystemManager::RegisterThread(this);
    }


    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:


    static constexpr ulong dayMillis = 86400000U; // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds;

    /// The nightly upload is the one chance to flush a day of data, so it waits
    /// noticeably longer than normal callers for room in the modem queue.
    static constexpr TickType_t shutdownEnqueueTimeout = pdMS_TO_TICKS(10000);

    ulong targetRestartTimeMs;

    ModemService& modem;

    ulong calculateTimeTillRestart(int hour, int minute, int second) const;
};
