#include "RestartTask.h"

#include "logging/Loggers.h"
#include "system/SystemManager.h"
#include "tasks/ModemTask.h"

void RestartTask::setup()
{
    logger.debugln("Restart task started");
}

void RestartTask::run()
{
    constexpr TickType_t waitForSystemTimeTimeout = pdMS_TO_TICKS(5 * 60 * 1000);

    const std::optional<ModemPayload> result = modem.waitFor(ModemResult::Timestamp, waitForSystemTimeTimeout);

    ulong restartTargetTimeMs;

    if (result.has_value())
    {
        const auto [hour, minute, second] = std::get<ModemTimestamp>(*result);
        restartTargetTimeMs = calculateTimeTillRestart(hour, minute, second);
    }
    else
    {
        restartTargetTimeMs = dayMillis;
        logger.warningln("Failed to get network time in time. Next restart in 24h");
    }

    logger.infoln("Next restart planned in " + String(restartTargetTimeMs / 60000) + " minutes");

    SystemManager::ReportReadyForRestart(m_id);

    const BaseType_t wokenByCommand = xTaskNotifyWait(0, 0xFFFFFFFF, nullptr, pdMS_TO_TICKS_LONG(restartTargetTimeMs));

    if (wokenByCommand == pdTRUE)
    {
        // interrupted early by system command. Stop restart routine
        return;
    }

    SystemManager::ReportUnReadyForRestart(m_id);

    logger.infoln("Time reached to upload log and restart ESP32");

    // The nightly flush must not be dropped, so these get no deadline and a
    // patient enqueue: losing them means losing a day of GPS data and logs.
    for (const ModemCommand cmd : {
             ModemCommand::Wakeup, ModemCommand::ConnectNetwork,
             ModemCommand::UploadGPSData, ModemCommand::UploadLog
         })
        modem.sendRequest(cmd, ModemRequest::noDeadline, shutdownEnqueueTimeout);

    SystemManager::ReportReadyForRestart(m_id);

    SystemManager::TriggerSystemHotRestart();
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
