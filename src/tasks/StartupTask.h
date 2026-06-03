#pragma once
#include <atomic>

#include "SystemThread.h"
#include "services/ModemService.h"


class StartupTask : public SystemThread
{
public:
    StartupTask(ModemService& modem, ImeiStore& imeiStore)
        : SystemThread(SystemThreadId::StartupTask, "STARTUP", 4096, ThreadPriority::StartupTask), modem(modem), imeiStore(imeiStore)
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
};
