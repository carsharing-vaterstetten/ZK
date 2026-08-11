#pragma once

#include <atomic>

#include "SystemThread.h"
#include "logic/SystemManager.h"
#include "modules/KeyControl.h"

enum class KeyControlCommand
{
    Lock,
    Unlock,
};


class KeyControlService : public SystemThread
{
public:
    explicit KeyControlService(const KeyControl& keyControl) : SystemThread(SystemThreadId::KeyControlService, "KEYCTRL", 4096,
                                                                            ThreadPriority::KeyControlService, 0), keyControl(keyControl)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;
    void lock() const;
    void unlock() const;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    const KeyControl& keyControl;

    QueueHandle_t cmdQueue = xQueueCreate(10, sizeof(KeyControlCommand));
};
