#pragma once
#include <atomic>

#include "logic/SystemManager.h"
#include "services/ModemService.h"

class RestartTask : public SystemThread
{
public:
    explicit RestartTask(const ulong targetRestartTimeMs, ModemService& modem)
        : SystemThread(SystemThreadId::RestartTask, "RSTRT", 4096, ThreadPriority::RestartTask), targetRestartTimeMs(targetRestartTimeMs),
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

    ulong targetRestartTimeMs;

    ModemService& modem;

    ulong calculateTimeTillRestart(int hour, int minute, int second) const;
};
