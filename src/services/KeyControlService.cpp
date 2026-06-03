#include "KeyControlService.h"

#include "modules/KeyControl.h"


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

void KeyControlService::lock() const
{
    constexpr auto cmd = KeyControlCommand::Lock;
    xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
}

void KeyControlService::unlock() const
{
    constexpr auto cmd = KeyControlCommand::Unlock;
    xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
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
