#pragma once
#include <atomic>

#include "system/SystemThread.h"
#include "tasks/ModemTask.h"
#include "LedSchedulerTask.h"

class StartupTask : public SystemThread
{
public:
    StartupTask(ModemTask& modem, const AccessStatus& accessStatus, LedSchedulerTask& led, const ApiClient& api)
        : SystemThread(SystemThreadId::StartupTask, "STARTUP", 4096, ThreadPriority::StartupTask, 0),
          modem(modem), accessStatus(accessStatus), led(led), api(api)
    {
        SystemManager::RegisterThread(this);
    }

    std::optional<uint> displayApiProgress(ModemState desiredState, uint32_t hexColor, TickType_t timeoutToReachDesiredState, TickType_t
                                           timeToCompleteDesiredState);

protected:
    void setup() override;
    void run() override;

private:
    /// The boot sequence is driven by watching the modem's state change, so this
    /// task polls. It only runs during boot, and the rate has to stay fine enough
    /// to catch short-lived states.
    static constexpr TickType_t pollInterval = pdMS_TO_TICKS(10);

    ModemTask& modem;
    LedSchedulerTask& led;
    const ApiClient& api;
    const AccessStatus& accessStatus;
};
