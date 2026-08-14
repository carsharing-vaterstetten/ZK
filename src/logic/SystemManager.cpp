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
    {
        if (t == nullptr) continue;

        if (!t->startTask())
        {
            fileLog.criticalln("Failed to start task " + String(t->getName()));
            continue;
        }

        serialOnlyLog.debugln("Task " + String(t->getName()) + " started");
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
    BroadCastCommand(SystemCommand::PrepareForHotRestart);

    // The task that triggered the restart is parked right here and can never
    // reach the end of its own run loop to report in. Without this it would wait
    // out the full timeout on itself every time — which is what made an OTA
    // restart always take the slow path.
    for (const SystemThread* t : m_registry)
        if (t != nullptr && t->getTaskHandle() == xTaskGetCurrentTaskHandle())
            ReportReadyForRestart(t->getId());

    constexpr EventBits_t expectedBits = (1 << static_cast<uint8_t>(SystemThreadId::Count)) - 1;
    EventBits_t receivedBits = xEventGroupWaitBits(m_lifecycleEventGroup, expectedBits, pdFALSE, pdTRUE, timeout);

    if ((receivedBits & expectedBits) != expectedBits)
        fileLog.warningln("Restart timed out waiting for tasks. Missing: " +
            String(expectedBits & ~receivedBits, 2));

    fileLog.debugln("Ended tasks: " + String(receivedBits, 2));

    fileLog.infoln("Restarting now");

    fileLog.flush();
    serialOnlyLog.flush();
    swLog.end(); // NO MORE FILE LOGGING FROM HERE

    ExecuteHotRestart();
}

void SystemManager::BroadCastCommand(const SystemCommand cmd)
{
    for (SystemThread* t : m_registry)
    {
        if (t == nullptr) continue;
        t->OnCommand(cmd);
    }
}

void SystemManager::ReportReadyForRestart(SystemThreadId id)
{
    xEventGroupSetBits(m_lifecycleEventGroup, 1 << static_cast<uint8_t>(id));
}

void SystemManager::ReportUnReadyForRestart(SystemThreadId id)
{
    xEventGroupClearBits(m_lifecycleEventGroup, 1 << static_cast<uint8_t>(id));
}

std::array<SystemThread*, SystemManager::taskCount> SystemManager::getAllTasks()
{
    return m_registry;
}


[[noreturn]] void SystemManager::ExecuteHotRestart()
{
    ESP.restart();
    while (true); // will never be reached
}
