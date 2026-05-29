#pragma once

#include <atomic>

#include "SystemThread.h"
#include "services/ModemService.h"
#include "shared/AccessStatus.h"
#include "shared/GPSAlg.h"


class GPSTask : SystemThread
{
public:
    GPSTask(const AccessStatus& accessStatus, ModemService& modem, GPSAlg& gpsAlg) : SystemThread(
            SystemThreadId::GPSTask, "GPSTask", 4096, 3), accessStatus(accessStatus), modem(modem), gpsAlg(gpsAlg) {}

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    const AccessStatus& accessStatus;
    ModemService& modem;
    GPSAlg& gpsAlg;
    GPSAlgPrediction lastGpsState = GPSAlgPrediction::Standing;

    void checkGPS();
};
