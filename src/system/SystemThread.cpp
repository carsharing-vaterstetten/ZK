#include "system/SystemThread.h"

#include "logging/Loggers.h"
#include "system/SystemManager.h"


SystemThread::~SystemThread()
{
    // TaskHook clears the handle when the task exits under its own power, so this
    // only fires when the object outlives a still-running task. Deleting a handle
    // whose task already called vTaskDelete(nullptr) frees the TCB twice.
    if (m_taskHandle != nullptr) vTaskDelete(m_taskHandle);
}

SystemThreadId SystemThread::getId() const
{
    return m_id;
}

void SystemThread::OnCommand(const SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::PrepareForHotRestart:
        m_running.store(false, std::memory_order_relaxed);
        notifySelf(); // so a task sleeping on a notification doesn't sit out its timeout
        break;
    case SystemCommand::None:
    case SystemCommand::EnterLowPower:
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

void SystemThread::notifySelf() const
{
    if (m_taskHandle != nullptr) xTaskNotifyGive(m_taskHandle);
}

bool SystemThread::startTask()
{
    if (xTaskCreatePinnedToCore(TaskHook, name, stackDepth, this, prio, &m_taskHandle, xCoreID) == pdPASS)
        return true;

    m_taskHandle = nullptr;
    return false;
}

TaskHandle_t SystemThread::getTaskHandle() const
{
    return m_taskHandle;
}

const char* SystemThread::getName() const
{
    return name;
}

void SystemThread::TaskHook(void* pvParams)
{
    auto* instance = static_cast<SystemThread*>(pvParams);

    logger.debugln("Task " + String(instance->name) + " started");

    instance->setup();
    instance->run();

    logger.debugln("Task " + String(instance->name) + " ended");

    // Reported here rather than at the end of every run() so no task can forget
    // and stall the restart handshake until it times out.
    SystemManager::ReportReadyForRestart(instance->m_id);

    instance->m_taskHandle = nullptr;
    vTaskDelete(nullptr);
}
