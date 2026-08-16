#pragma once
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
    AccessControlTask(const RFIDs& rfidsManager, GPSAlg& gpsAlg,
                      KeyControlService& keyControlService, AccessStatus& accessStatus,
                      LedSchedulerTask& led, ModemService& modem, const CardReaderService& cardReader)
        : SystemThread(SystemThreadId::AccessControlTask, "ACCTRL", 4096, ThreadPriority::AccessControlTask, 0),
          rfidsManager(rfidsManager),
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
    /// toggles the car open/closed indefinitely. It doubles as the debounce after
    /// a clean tap, so keeping it short is what stops the reader from feeling
    /// dead right after it reacted.
    static constexpr TickType_t cardRemovalCooldown = pdMS_TO_TICKS(1000);

    /// A card seen more recently than this counts as still lying on the reader.
    /// Has to clear more than one reader poll (the PN532 read blocks for up to
    /// 200ms) so a missed beat isn't mistaken for the card being taken away.
    static constexpr TickType_t cardPresenceWindow = pdMS_TO_TICKS(500);

    /// How long the card may rest on the reader before the LED starts asking for
    /// it back. Must exceed cardPresenceWindow, otherwise a card that was already
    /// removed still looks present when the grace period expires.
    static constexpr TickType_t cardHeldGracePeriod = pdMS_TO_TICKS(1000);

    static constexpr uint32_t cardRemovalIndicatorColor = 0x3fd0d4;

    /// How long the strip takes to fade away once the card is lifted. Runs from
    /// the moment removal can first be detected to the moment the reader
    /// re-arms, so the light reaching black means "ready for the next card"
    /// rather than being an arbitrary animation length.
    static constexpr TickType_t cardFadeOutDuration = cardRemovalCooldown - cardPresenceWindow;
    static_assert(cardRemovalCooldown > cardPresenceWindow, "fade-out window would be empty or negative");

    /// Anything to do with a card physically on the reader outranks ambient
    /// status output. Must match the lock/unlock flashes so those still play in
    /// order ahead of the countdown rather than being pushed aside by it — equal
    /// priority is FIFO, and the flash is always queued first.
    static constexpr LedPriority cardFeedbackPriority = LedPriority::High;

    /// GPS wakeup is a nice-to-have on the unlock path. If the modem is busy
    /// enough that this goes stale, dropping it is correct — the trip is still
    /// recorded, and the user is not left waiting at the car.
    static constexpr TickType_t gpsWakeupTimeToLive = pdMS_TO_TICKS(30000);

    std::atomic<bool> m_running = true;

    const RFIDs& rfidsManager;
    GPSAlg& gpsAlg;
    KeyControlService& keyControlService;
    AccessStatus& accessStatus;
    LedSchedulerTask& led;
    ModemService& modem;
    const CardReaderService& cardReader;

    void doThings(uint32_t rfidUid);

    /// Blocks until the card has been away from the reader for
    /// `cardRemovalCooldown`, prompting once it is clear the user has left it
    /// there.
    void waitForCardRemoval();

    /// The three phases of removal feedback: silent while it is still unclear
    /// whether this was a tap, pulsing while the card is being left on the
    /// reader, then draining while the reader re-arms.
    static ProgressState cardRemovalFeedback(bool prompting, bool cardStillOnReader, TickType_t absentFor);
};
