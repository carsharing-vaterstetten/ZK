#pragma once

#include <Arduino.h>

#include "system/SystemTypes.h"

class SystemThread
{
public:
    explicit SystemThread(const SystemThreadId id, const char* name, const uint32_t stackDepth, const ThreadPriority prio,  const int xCoreID) :
        m_id(id), name(name), stackDepth(stackDepth), prio(static_cast<UBaseType_t>(prio)), xCoreID(xCoreID) {}

    virtual ~SystemThread();

    [[nodiscard]] SystemThreadId getId() const;

    // System Manager uses this to inject lifecycle events directly into the task
    virtual void OnCommand(SystemCommand cmd) = 0;

    /// Returns false if the task could not be created (out of heap).
    bool startTask();

    [[nodiscard]] TaskHandle_t getTaskHandle() const;
    [[nodiscard]] const char* getName() const;

protected:
    TaskHandle_t m_taskHandle = nullptr;
    SystemThreadId m_id;
    const char* name;
    uint32_t stackDepth;
    UBaseType_t prio;
    const int xCoreID;

    // Core lifecycles
    virtual void setup() = 0;
    virtual void run() = 0;

private:
    static void TaskHook(void* pvParams);
};
