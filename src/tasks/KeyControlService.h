#pragma once

#include <atomic>

#include "system/SystemThread.h"
#include "system/SystemManager.h"
#include "hal/KeyControl.h"

enum class KeyControlCommand
{
    Lock,
    Unlock,
};


class KeyControlService : public SystemThread
{
public:
    explicit KeyControlService(KeyControl& keyControl) : SystemThread(SystemThreadId::KeyControlService, "KEYCTRL", 4096,
                                                                      ThreadPriority::KeyControlService, 0), keyControl(keyControl)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

    /// Queues the key sequence. Returns false if it could not be queued, so the
    /// caller can avoid signalling success for something that never ran.
    bool lock();
    bool unlock();

protected:
    void setup() override;
    void run() override;

private:
    static constexpr TickType_t enqueueTimeout = pdMS_TO_TICKS(1000);

    std::atomic<bool> m_running = true;

    KeyControl& keyControl;

    QueueHandle_t cmdQueue = xQueueCreate(10, sizeof(KeyControlCommand));

    bool send(KeyControlCommand cmd);
};
