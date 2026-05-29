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
    const ModemTxMessage* msg = modem.waitForSpecificModemMessage(ModemTxDataType::Timestamp);
    const auto [hour, minute, second] = std::get<ModemTimestamp>(*msg->payload);
    const ulong restartTargetMs = calculateNextRestartTime(hour, minute, second);
    delete msg;

    fileLog.infoln("Next restart planned in " + String(restartTargetMs / 1000) + " seconds");

    while (m_running)
    {
        if (millis() >= restartTargetMs)
        {
            restartRoutine();
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }

    SystemManager::ReportReadyForRestart(m_id);
    vTaskDelete(nullptr);
}

void RestartTask::OnCommand(SystemCommand cmd)
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

void RestartTask::restartRoutine()
{
    fileLog.infoln("Time reached to upload log and restart ESP32");

    modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::Wakeup);
    modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::ConnectNetwork);
    modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::UploadGPSData);
    modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::UploadLog);
}

ulong RestartTask::calculateNextRestartTime(const int hour, const int minute, const int second) const
{
    // Calculate milliseconds since midnight
    ulong targetTimeMs;

    const ulong timeOfDayInMs = (hour * 3600 + minute * 60 + second) * 1000;

    if (timeOfDayInMs < targetRestartTimeMs)
        targetTimeMs = targetRestartTimeMs - timeOfDayInMs;
    else
        targetTimeMs = dayMillis - (timeOfDayInMs - targetRestartTimeMs);

    targetTimeMs += millis();

    return targetTimeMs;
}
