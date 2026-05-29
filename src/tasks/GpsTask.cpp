//
// Created by finnd on 28.05.2026.
//

#include "GpsTask.h"

#include <LittleFS.h>

#include "config/user_config.h"
#include "logic/SystemManager.h"
#include "services/ModemService.h"
#include "shared/Globals.h"

enum class GPSAlgPrediction;

void GPSTask::OnCommand(SystemCommand cmd)
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

void GPSTask::checkGPS()
{
    if (LittleFS.totalBytes() - LittleFS.usedBytes() < 128 * 1024)
    {
        // GPS is logging to flash and storage is low
        serialOnlyLog.warningln("Low on flash storage. Not logging GPS");
        return;
    }

    modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::GetGPSData);
    const ModemTxMessage* msg = modem.waitForSpecificModemMessage(ModemTxDataType::GPSData);
    const GPS_DATA_t gpsData = std::get<GPS_DATA_t>(*msg->payload);
    delete msg;

    const GPSAlgPrediction gpsState = gpsAlg.pushData(gpsData);

    if (gpsState != lastGpsState)
    {
        fileLog.infoln("Car state changed to " + GPSAlg::gpsAlgPredictionToStr(gpsState));
        lastGpsState = gpsState;
    }
}

void GPSTask::setup()
{
    fileLog.debugln("GPS data task started");
}

void GPSTask::run()
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (m_running)
    {
        TickType_t xFrequency;

        if (accessStatus.isLoggedIn())
        {
            xFrequency = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_DRIVING);
            if (accessStatus.givesGPSTrackingPermission().value_or(true))
                checkGPS();
        }
        else
        {
            xFrequency = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_STANDING);
            if constexpr (RECORD_GPS_WHILE_STANDING)
                checkGPS();
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    SystemManager::ReportReadyForRestart(m_id);
}
