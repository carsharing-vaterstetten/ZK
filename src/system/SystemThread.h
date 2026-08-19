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
    /// `watchdogCritical` subscribes the task to the hardware task watchdog
    /// (see system/Watchdog.h) for its entire lifetime, from before setup()
    /// runs to after run() returns. Reserved for tasks whose hang would strand
    /// a user or otherwise need a hardware reset to recover from — most tasks
    /// should leave this false.
    explicit SystemThread(const SystemThreadId id, const char* name, const uint32_t stackDepth, const ThreadPriority prio,  const int xCoreID,
                          const bool watchdogCritical = false) :
        m_id(id), name(name), stackDepth(stackDepth), prio(static_cast<UBaseType_t>(prio)), xCoreID(xCoreID),
        m_watchdogCritical(watchdogCritical) {}

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
    const bool m_watchdogCritical;

    [[nodiscard]] bool isRunning() const { return m_running.load(std::memory_order_relaxed); }

    /// Wakes the task if it is blocked on a notification. Safe before startTask().
    void notifySelf() const;

    /// Resets the task watchdog on this task's behalf. A no-op if the task was
    /// not constructed with watchdogCritical = true — safe to call unconditionally
    /// from run() without checking the flag yourself.
    void feedWatchdog() const;

    // Core lifecycles
    virtual void setup() = 0;
    virtual void run() = 0;

private:
    std::atomic<bool> m_running{true};

    static void TaskHook(void* pvParams);
};
