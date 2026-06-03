#include "StartupTask.h"

#include "config/user_config.h"
#include "logic/HelperUtils.h"
#include "logic/SystemManager.h"
#include "shared/Globals.h"

void StartupTask::OnCommand(SystemCommand cmd)
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

void StartupTask::setup()
{
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::GetImei);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::ConnectNetwork);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::GetUnixTime);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::GetTimestamp);

    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::PerformConnectionSpeedTest);

    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
        modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::DoFirmwareUpdateIfAvailable);
    else
        fileLog.infoln("Skipped firmware update check");

    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::DownloadRfidIfChanged);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::DownloadGPSRfids);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::UploadLog);

    // Power saving
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::DisconnectNetwork);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::SleepIfPossible);
}


void StartupTask::run()
{
    SystemManager::ReportReadyForRestart(m_id);
}
