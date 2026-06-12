#include "SystemManager.h"

#include "shared/Globals.h"

void SystemManager::Init()
{
    m_lifecycleEventGroup = xEventGroupCreate();
    m_registry.fill(nullptr);
}

void SystemManager::Start()
{
    for (SystemThread* t : m_registry)
        if (t != nullptr)
        {
            serialOnlyLog.debugln("Task " + String((uint8_t)t->getId()) + " started");
            t->startTask();
        }
}

void SystemManager::RegisterThread(SystemThread* thread)
{
    const auto index = static_cast<size_t>(thread->getId());
    if (index < m_registry.size())
        m_registry[index] = thread;
}

[[noreturn]] void SystemManager::TriggerSystemHotRestart(TickType_t timeout)
{
    for (SystemThread* t : m_registry)
    {
        if (t == nullptr) continue;
        t->OnCommand(SystemCommand::PrepareForHotRestart);
    }

    constexpr EventBits_t expectedBits = (1 << static_cast<uint8_t>(SystemThreadId::Count)) - 1;
    EventBits_t receivedBits = xEventGroupWaitBits(m_lifecycleEventGroup, expectedBits, pdFALSE, pdTRUE, timeout);

    fileLog.debugln("Ended tasks: " + String(receivedBits, 2));

    fileLog.infoln("Restarting now");

    fileLog.flush();
    serialOnlyLog.flush();
    swLog.end(); // NO MORE FILE LOGGING FROM HERE

    ExecuteHotRestart();
}

void SystemManager::ReportReadyForRestart(SystemThreadId id)
{
    xEventGroupSetBits(m_lifecycleEventGroup, 1 << static_cast<uint8_t>(id));
}

void SystemManager::ReportUnReadyForRestart(SystemThreadId id)
{
    xEventGroupClearBits(m_lifecycleEventGroup, 1 << static_cast<uint8_t>(id));
}


[[noreturn]] void SystemManager::ExecuteHotRestart()
{
    ESP.restart();
    while (true); // will never be reached
}
