#pragma once
#include <atomic>

#include "SystemThread.h"
#include "services/ModemService.h"
#include "services/LedService.h"

class StartupTask : public SystemThread
{
public:
    StartupTask(ModemService& modem, ImeiStore& imeiStore, const AccessStatus& accessStatus, LedService& led)
        : SystemThread(SystemThreadId::StartupTask, "STARTUP", 4096, ThreadPriority::StartupTask), modem(modem), imeiStore(imeiStore), accessStatus(accessStatus), led(led)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    ModemService& modem;
    ImeiStore& imeiStore;
    const AccessStatus& accessStatus;
    LedService& led;
};
