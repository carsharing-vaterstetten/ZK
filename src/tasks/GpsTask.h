#pragma once

#include <atomic>

#include "SystemThread.h"
#include "services/ModemService.h"
#include "shared/AccessStatus.h"
#include "shared/GPSAlg.h"


class GPSTask : SystemThread
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
    std::atomic<bool> m_running = true;

    TickType_t currentGPSPollingTime;

    const AccessStatus& accessStatus;
    ModemService& modem;
    GPS& gps;
    GPSAlg& gpsAlg;
    GPSAlgPrediction lastGpsState = GPSAlgPrediction::Standing;

    void checkGPS();
};
