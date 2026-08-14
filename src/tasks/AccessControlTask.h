#pragma once
#include "config/hw_config.h"
#include "logic/SystemManager.h"
#include "services/CardReaderService.h"
#include "services/KeyControlService.h"
#include "LedSchedulerTask.h"
#include "services/ModemService.h"
#include "shared/GPSAlg.h"
#include "shared/RFIDs.h"

struct ScanResult;

class AccessControlTask : public SystemThread
{
public:
    AccessControlTask(const BoardConfig& board, const RFIDs& rfidsManager, GPSAlg& gpsAlg,
                      KeyControlService& keyControlService, AccessStatus& accessStatus,
                       LedSchedulerTask& led, ModemService& modem, const CardReaderService& cardReader)
        : SystemThread(SystemThreadId::AccessControlTask, "ACCTRL", 4096, ThreadPriority::AccessControlTask, 0),
          board(board), rfidsManager(rfidsManager),
          gpsAlg(gpsAlg), keyControlService(keyControlService), accessStatus(accessStatus), led(led), modem(modem),
          cardReader(cardReader)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    /// A scan older than this is a leftover from a previous tap rather than a new
    /// one. It has to comfortably exceed the reader's poll period (the PN532 read
    /// blocks for up to 200ms) plus scheduling jitter, or taps get dropped
    /// whenever something else on core 0 holds the CPU for a moment.
    static constexpr TickType_t scanFreshnessWindow = pdMS_TO_TICKS(500);

    /// The card has to be off the reader this long before another tap counts.
    /// Without it, a card left lying on the reader re-triggers every cycle and
    /// toggles the car open/closed indefinitely.
    static constexpr TickType_t cardRemovalCooldown = pdMS_TO_TICKS(3000);

    /// How long the card may rest on the reader before the LED starts asking for
    /// it back. Covers the normal case of a user holding it a moment too long.
    static constexpr TickType_t cardHeldGracePeriod = pdMS_TO_TICKS(1000);

    static constexpr uint32_t cardRemovalIndicatorColor = 0x3fd0d4;

    /// GPS wakeup is a nice-to-have on the unlock path. If the modem is busy
    /// enough that this goes stale, dropping it is correct — the trip is still
    /// recorded, and the user is not left waiting at the car.
    static constexpr TickType_t gpsWakeupTimeToLive = pdMS_TO_TICKS(30000);

    std::atomic<bool> m_running = true;

    const BoardConfig& board;
    const RFIDs& rfidsManager;
    GPSAlg& gpsAlg;
    KeyControlService& keyControlService;
    AccessStatus& accessStatus;
    LedSchedulerTask& led;
    ModemService& modem;
    const CardReaderService& cardReader;

    void doThings(uint32_t rfidUid);

    /// Blocks until the card has been away from the reader for
    /// `cardRemovalCooldown`, showing a countdown once it is clear the user has
    /// left it there.
    void waitForCardRemoval();
};
