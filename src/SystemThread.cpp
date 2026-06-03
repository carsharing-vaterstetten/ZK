#include "SystemThread.h"


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
    xTaskCreate(TaskHook, name, stackDepth, this, prio, &m_taskHandle);
}

void SystemThread::TaskHook(void* pvParams)
{
    auto* instance = static_cast<SystemThread*>(pvParams);

    instance->setup();
    instance->run();

    vTaskDelete(nullptr);
}
