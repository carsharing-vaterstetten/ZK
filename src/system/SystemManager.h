#pragma once

#include "system/SystemThread.h"
#include "system/SystemTypes.h"


class SystemManager
{
public:
    SystemManager() = delete;
    static constexpr size_t taskCount = static_cast<size_t>(SystemThreadId::Count);

    static void Start();

    static void RegisterThread(SystemThread* thread);

    /// Any task can call this to initiate an orderly system reboot
    [[noreturn]] static void TriggerSystemHotRestart(TickType_t timeout = pdMS_TO_TICKS(20000));
    static void BroadCastCommand(SystemCommand cmd);
    static void ReportReadyForRestart(SystemThreadId id);
    static void ReportUnReadyForRestart(SystemThreadId id);

    static std::array<SystemThread*, taskCount> getAllTasks();

private:

    // Both are ready before any constructor runs: m_registry is zero-initialised
    // at static-init time and tasks register from their own constructors, so
    // there is no init step that could clear the registry after the fact.
    static inline std::array<SystemThread*, taskCount> m_registry{};
    static inline EventGroupHandle_t m_lifecycleEventGroup = xEventGroupCreate();


    [[noreturn]] static void ExecuteHotRestart();
};
