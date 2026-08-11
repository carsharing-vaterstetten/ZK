#include "SystemThread.h"

#include "shared/Globals.h"


SystemThread::~SystemThread()
{
    if (m_taskHandle != nullptr) vTaskDelete(m_taskHandle);
}

SystemThreadId SystemThread::getId() const
{
    return m_id;
}

void SystemThread::startTask()
{
    xTaskCreatePinnedToCore(TaskHook, name, stackDepth, this, prio, &m_taskHandle, xCoreID);
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

    fileLog.debugln("Task " + String(instance->name) + " started");

    instance->setup();
    instance->run();

    fileLog.debugln("Task " + String(instance->name) + " ended");

    vTaskDelete(nullptr);
}
