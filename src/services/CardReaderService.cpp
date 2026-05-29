#include "CardReaderService.h"

#include "shared/SharedData.h"

void CardReaderService::OnCommand(const SystemCommand cmd)
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

void CardReaderService::setup()
{
}

void CardReaderService::run()
{
    static ScanResult scanResult;

    while (m_running)
    {
        scanResult = cardReader.scan(false);
        SharedData::registerData(newCardScannedId, &scanResult);
    }

    SystemManager::ReportReadyForRestart(m_id);
}
