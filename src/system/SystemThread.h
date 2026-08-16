#pragma once

#include <atomic>
#include <Arduino.h>

#include "system/SystemTypes.h"

/// Base for every FreeRTOS task. Subclasses implement setup() and run(); the
/// shutdown handshake is handled here — run() loops on isRunning(), and
/// readiness is reported once it returns.
class SystemThread
{
public:
    explicit SystemThread(const SystemThreadId id, const char* name, const uint32_t stackDepth, const ThreadPriority prio,  const int xCoreID) :
        m_id(id), name(name), stackDepth(stackDepth), prio(static_cast<UBaseType_t>(prio)), xCoreID(xCoreID) {}

    virtual ~SystemThread();

    [[nodiscard]] SystemThreadId getId() const;

    /// Called by SystemManager on the *broadcaster's* thread, not this one, so
    /// implementations may only touch atomics and thread-safe primitives.
    /// The default stops the run loop and wakes the task; override to add to it.
    virtual void OnCommand(SystemCommand cmd);

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

    [[nodiscard]] bool isRunning() const { return m_running.load(std::memory_order_relaxed); }

    /// Wakes the task if it is blocked on a notification. Safe before startTask().
    void notifySelf() const;

    // Core lifecycles
    virtual void setup() = 0;
    virtual void run() = 0;

private:
    std::atomic<bool> m_running{true};

    static void TaskHook(void* pvParams);
};
