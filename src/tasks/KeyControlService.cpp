#include "tasks/KeyControlService.h"

#include "hal/KeyControl.h"
#include "logging/Loggers.h"


void KeyControlService::OnCommand(const SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        break;
    case SystemCommand::EnterLowPower:
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

bool KeyControlService::send(const KeyControlCommand cmd)
{
    if (xQueueSend(cmdQueue, &cmd, enqueueTimeout) == pdTRUE)
        return true;

    fileLog.errorln("Key control queue full, command dropped");
    return false;
}

bool KeyControlService::lock()
{
    return send(KeyControlCommand::Lock);
}

bool KeyControlService::unlock()
{
    return send(KeyControlCommand::Unlock);
}

void KeyControlService::setup() {}

void KeyControlService::run()
{
    KeyControlCommand cmd;

    while (m_running)
    {
        if (xQueueReceive(cmdQueue, &cmd, pdMS_TO_TICKS(500)) == pdFALSE)
            continue;

        switch (cmd)
        {
        case KeyControlCommand::Lock:
            keyControl.lock();
            break;
        case KeyControlCommand::Unlock:
            keyControl.unlock();
            break;
        }
    }

    SystemManager::ReportReadyForRestart(m_id);
}
