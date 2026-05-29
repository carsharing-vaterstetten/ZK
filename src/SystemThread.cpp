#include "SystemThread.h"


SystemThread::SystemThread(const SystemThreadId id, const char* name, const uint32_t stackDepth,
                               const UBaseType_t prio) : m_id(id)
{
    xTaskCreate(TaskHook, name, stackDepth, this, prio, &m_taskHandle);
}

SystemThread::~SystemThread()
{
    if (m_taskHandle != nullptr) vTaskDelete(m_taskHandle);
}

SystemThreadId SystemThread::getId() const
{
    return m_id;
}

void SystemThread::TaskHook(void* pvParams)
{
    auto* instance = static_cast<SystemThread*>(pvParams);

    instance->setup();
    instance->run();
}
