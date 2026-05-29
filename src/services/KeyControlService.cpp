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

void KeyControlService::toggleLogin(const uint32_t uid) const
{
    xQueueSend(uidQueue, &uid, portMAX_DELAY);
}

void KeyControlService::setup()
{
    keyControl.begin();
}

void KeyControlService::run()
{
    while (m_running)
    {
        uint32_t uid;
        xQueueReceive(uidQueue, &uid, portMAX_DELAY);
        keyControl.toggleLogin(uid);
    }

    SystemManager::ReportReadyForRestart(m_id);
}
