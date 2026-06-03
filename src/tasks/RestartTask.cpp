#include "RestartTask.h"

#include "shared/Globals.h"
#include "logic/SystemManager.h"
#include "services/ModemService.h"

void RestartTask::setup()
{
    fileLog.debugln("Restart task started");
}


void RestartTask::run()
{
    constexpr TickType_t waitForSystemTimeTimeout = pdMS_TO_TICKS(5 * 60 * 1000);

    const ModemTxMessage* msg = modem.waitForSpecificMessage(ModemTxDataType::Timestamp, waitForSystemTimeTimeout);

    ulong restartTargetTimeMs;


    if (msg == nullptr)
    {
        restartTargetTimeMs = 24 * 60 * 60 * 1000;
        fileLog.warningln("Failed to get network time in time. Next restart in 24h");
    }
    else
    {
        const ModemTimestamp time = std::get<ModemTimestamp>(*msg->payload);
        restartTargetTimeMs = calculateTimeTillRestart(time.hour, time.minute, time.second);
    }

    delete msg;

    fileLog.infoln("Next restart planned in " + String(restartTargetTimeMs / 60000) + " minutes");
    fileLog.infoln("Next restart planned in " + String(pdMS_TO_TICKS_LONG(restartTargetTimeMs)) + " ticks");

    SystemManager::ReportReadyForRestart(m_id);

    const BaseType_t wokenByCommand = xTaskNotifyWait(0, 0xFFFFFFFF, nullptr, pdMS_TO_TICKS_LONG(restartTargetTimeMs));

    if (wokenByCommand == pdTRUE)
    {
        // interrupted early by system command.
        return;
    }

    SystemManager::ReportUnReadyForRestart(m_id);

    fileLog.infoln("Time reached to upload log and restart ESP32");

    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::Wakeup);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::ConnectNetwork);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::UploadGPSData);
    modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::UploadLog);


    SystemManager::ReportReadyForRestart(m_id);

    SystemManager::TriggerSystemHotRestart();
}

void RestartTask::OnCommand(SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        xTaskNotify(m_taskHandle, 0, eSetBits);
        break;
    case SystemCommand::EnterLowPower:
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

ulong RestartTask::calculateTimeTillRestart(const int hour, const int minute, const int second) const
{
    // Calculate milliseconds since midnight
    ulong targetTimeMs;

    const ulong timeOfDayInMs = (hour * 3600 + minute * 60 + second) * 1000;

    if (timeOfDayInMs < targetRestartTimeMs)
        targetTimeMs = targetRestartTimeMs - timeOfDayInMs; // target time is today
    else
        targetTimeMs = dayMillis - (timeOfDayInMs - targetRestartTimeMs); // target time is tomorrow

    return targetTimeMs;
}
