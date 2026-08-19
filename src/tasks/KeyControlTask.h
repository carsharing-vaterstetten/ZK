#pragma once

#include <atomic>

#include "system/SystemThread.h"
#include "hal/CarKey.h"
#include "system/SystemManager.h"

enum class KeyControlCommand
{
    Lock,
    Unlock,
};

class KeyControlTask : public SystemThread
{
public:
    explicit KeyControlTask(CarKey& carKey) : SystemThread(SystemThreadId::KeyControlTask, "KEYCTRL", 4096,
                                                                      ThreadPriority::KeyControlTask, 0,
                                                                      /*watchdogCritical=*/true), carKey(carKey)
    {
        SystemManager::RegisterThread(this);
    }

    /// Queues the key sequence. Returns false if it could not be queued, so the
    /// caller can avoid signalling success for something that never ran.
    bool lock();
    bool unlock();

protected:
    void setup() override;
    void run() override;

private:
    static constexpr TickType_t enqueueTimeout = pdMS_TO_TICKS(1000);

    CarKey& carKey;

    QueueHandle_t cmdQueue = xQueueCreate(10, sizeof(KeyControlCommand));

    bool send(KeyControlCommand cmd);
};
