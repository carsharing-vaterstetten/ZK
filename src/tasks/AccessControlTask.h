#pragma once
#include "config/hw_config.h"
#include "logic/SystemManager.h"
#include "services/KeyControlService.h"
#include "services/LedService.h"
#include "services/ModemService.h"
#include "shared/GPSAlg.h"
#include "shared/RFIDs.h"

struct ScanResult;

class AccessControlTask : public SystemThread
{
public:
    AccessControlTask(const BoardConfig& board, const RFIDs& rfidsManager, GPSAlg& gpsAlg,
                      const KeyControlService& keyControlService, const AccessStatus& accessStatus,
                      const LedService& led, ModemService& modem)
        : SystemThread(SystemThreadId::AccessControlTask, "ACCTRL", 4096, 1), board(board), rfidsManager(rfidsManager),
          gpsAlg(gpsAlg), keyControlService(keyControlService), accessStatus(accessStatus), led(led), modem(modem)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    const BoardConfig& board;
    const RFIDs& rfidsManager;
    GPSAlg& gpsAlg;
    const KeyControlService& keyControlService;
    const AccessStatus& accessStatus;
    const LedService& led;
    ModemService& modem;

    static ScanResult scan(TickType_t timeout);
    void doThings(uint32_t rfidUid);
    void cooldownSequence() const;
};
