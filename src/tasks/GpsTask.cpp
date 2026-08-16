//
// Created by finnd on 28.05.2026.
//

#include "GpsTask.h"

#include <LittleFS.h>

#include "config/user_config.h"
#include "system/SystemManager.h"
#include "tasks/ModemTask.h"
#include "logging/Loggers.h"

enum class MotionState;

void GpsTask::checkGPS()
{
    if (LittleFS.totalBytes() - LittleFS.usedBytes() < 128 * 1024)
    {
        // GPS is logging to flash and storage is low
        serialLogger.warningln("Low on flash storage. Not logging GPS");
        return;
    }

    if (!modem.sendRequest(ModemTaskCommand::GetGPSData, gpsRequestTimeToLive)) return;

    const std::unique_ptr<ModemTxMessage> msg = modem.waitForSpecificMessage(ModemTxDataType::GPSData, gpsReplyTimeout);

    if (msg == nullptr)
    {
        //serialLogger.debugln("No gps data");
        return;
    }

    const GPS_DATA_t gpsData = std::get<GPS_DATA_t>(*msg->payload);

    const MotionState gpsState = tripTracker.pushData(gpsData);
    gps.writeData(gpsData);

    if (gpsState != lastGpsState)
    {
        logger.infoln("Car state changed to " + TripTracker::motionStateToString(gpsState));
        lastGpsState = gpsState;
    }
}

void GpsTask::setup()
{
    while (modem.isWorkingOnTasks())
        vTaskDelay(pdMS_TO_TICKS(100));
}

void GpsTask::run()
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (isRunning())
    {
        if (accessStatus.isLoggedIn())
        {
            currentGPSPollingTime = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_DRIVING);
            if (accessStatus.givesGPSTrackingPermission().value_or(true))
                checkGPS();
        }
        else
        {
            currentGPSPollingTime = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_STANDING);
            if constexpr (RECORD_GPS_WHILE_STANDING)
                checkGPS();
        }

        vTaskDelayUntil(&xLastWakeTime, currentGPSPollingTime);
    }
}
