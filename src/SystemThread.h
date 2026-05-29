#pragma once

#include <Arduino.h>

#include "SystemTypes.h"

class SystemThread
{
public:
    explicit SystemThread(SystemThreadId id, const char* name, uint32_t stackDepth, UBaseType_t prio);

    virtual ~SystemThread();

    SystemThreadId getId() const;

    // System Manager uses this to inject lifecycle events directly into the task
    virtual void OnCommand(SystemCommand cmd) = 0;

protected:
    TaskHandle_t m_taskHandle = nullptr;
    SystemThreadId m_id;

    // Core lifecycles
    virtual void setup() = 0;
    virtual void run() = 0;

private:
    static void TaskHook(void* pvParams);
};
