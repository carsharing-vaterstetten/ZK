#pragma once

#include <atomic>

#include "system/SystemThread.h"
#include "config/user_config.h"
#include "tasks/ModemTask.h"
#include "domain/AccessStatus.h"
#include "domain/TripTracker.h"


class GpsTask : public SystemThread
{
public:
    GpsTask(const AccessStatus& accessStatus, ModemTask& modem, TripTracker& tripTracker, GpsLog& gps) : SystemThread(
            SystemThreadId::GpsTask, "GpsTask", 4096, ThreadPriority::GpsTask, 0), accessStatus(accessStatus), modem(modem), gps(gps),
        tripTracker(tripTracker)
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
    ModemTask& modem;
    GpsLog& gps;
    TripTracker& tripTracker;
    MotionState lastGpsState = MotionState::Standing;

    void checkGPS();
};
