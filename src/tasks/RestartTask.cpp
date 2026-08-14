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

    const std::unique_ptr<ModemTxMessage> msg = modem.waitForSpecificMessage(
        ModemTxDataType::Timestamp, waitForSystemTimeTimeout);

    ulong restartTargetTimeMs;

    if (msg == nullptr)
    {
        restartTargetTimeMs = dayMillis;
        fileLog.warningln("Failed to get network time in time. Next restart in 24h");
    }
    else
    {
        const ModemTimestamp time = std::get<ModemTimestamp>(*msg->payload);
        restartTargetTimeMs = calculateTimeTillRestart(time.hour, time.minute, time.second);
    }

    fileLog.infoln("Next restart planned in " + String(restartTargetTimeMs / 60000) + " minutes");

    SystemManager::ReportReadyForRestart(m_id);

    const BaseType_t wokenByCommand = xTaskNotifyWait(0, 0xFFFFFFFF, nullptr, pdMS_TO_TICKS_LONG(restartTargetTimeMs));

    if (wokenByCommand == pdTRUE)
    {
        // interrupted early by system command. Stop restart routine
        return;
    }

    SystemManager::ReportUnReadyForRestart(m_id);

    fileLog.infoln("Time reached to upload log and restart ESP32");

    // The nightly flush must not be dropped, so these get no deadline and a
    // patient enqueue: losing them means losing a day of GPS data and logs.
    for (const ModemTaskCommand cmd : {
             ModemTaskCommand::Wakeup, ModemTaskCommand::ConnectNetwork,
             ModemTaskCommand::UploadGPSData, ModemTaskCommand::UploadLog
         })
        modem.sendRequest(cmd, ModemRequest::noDeadline, shutdownEnqueueTimeout);


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
        // Broadcast can arrive before this task has been created.
        if (m_taskHandle != nullptr) xTaskNotify(m_taskHandle, 0, eSetBits);
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
