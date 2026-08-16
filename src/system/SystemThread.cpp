#include "system/SystemThread.h"

#include "logging/Loggers.h"


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

    instance->m_taskHandle = nullptr;
    vTaskDelete(nullptr);
}
