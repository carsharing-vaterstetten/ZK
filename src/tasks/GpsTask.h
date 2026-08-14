#pragma once

#include <atomic>

#include "SystemThread.h"
#include "config/user_config.h"
#include "services/ModemService.h"
#include "shared/AccessStatus.h"
#include "shared/GPSAlg.h"


class GPSTask : public SystemThread
{
public:
    GPSTask(const AccessStatus& accessStatus, ModemService& modem, GPSAlg& gpsAlg, GPS& gps) : SystemThread(
            SystemThreadId::GPSTask, "GPSTask", 4096, ThreadPriority::GPSTask, 0), accessStatus(accessStatus), modem(modem), gps(gps),
        gpsAlg(gpsAlg)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    /// How long a GPS request is worth running for. Tied to the polling interval:
    /// if the modem is tied up longer than that, the sample would be stale on
    /// arrival and the next poll supersedes it anyway. Keeping the deadline short
    /// is what stops this task from filling the modem queue with dead work while
    /// an OTA or log upload is in progress.
    static constexpr TickType_t gpsRequestTimeToLive = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_STANDING);

    static constexpr TickType_t gpsReplyTimeout = pdMS_TO_TICKS(5000);

    std::atomic<bool> m_running = true;

    TickType_t currentGPSPollingTime = 0;

    const AccessStatus& accessStatus;
    ModemService& modem;
    GPS& gps;
    GPSAlg& gpsAlg;
    GPSAlgPrediction lastGpsState = GPSAlgPrediction::Standing;

    void checkGPS();
};
