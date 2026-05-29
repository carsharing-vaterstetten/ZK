#pragma once

#include <atomic>

#include "SystemThread.h"
#include "logic/SystemManager.h"
#include "modules/KeyControl.h"

class KeyControlService : public SystemThread
{
public:
    KeyControlService(const KeyControl& keyControl) : SystemThread(SystemThreadId::KeyControlService, "KEYCTRL", 4096,
                                                                   2), keyControl(keyControl)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;
    void toggleLogin(uint32_t uid) const;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    const KeyControl& keyControl;

    QueueHandle_t uidQueue = xQueueCreate(10, sizeof(uint32_t));
};
