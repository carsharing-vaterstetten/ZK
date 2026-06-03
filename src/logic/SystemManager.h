#pragma once

#include "SystemThread.h"
#include "../SystemTypes.h"


class SystemManager
{
public:
    SystemManager() = delete;

    static void Init();
    static void Start();

    static void RegisterThread(SystemThread* thread);

    /// Any task can call this to initiate an orderly system reboot
    [[noreturn]] static void TriggerSystemHotRestart();
    static void ReportReadyForRestart(SystemThreadId id);
    static void ReportUnReadyForRestart(SystemThreadId id);

private:
    static inline std::array<SystemThread*, static_cast<size_t>(SystemThreadId::Count)> m_registry{};
    static inline EventGroupHandle_t m_lifecycleEventGroup = nullptr;

    [[noreturn]] static void ExecuteHotRestart();
};
