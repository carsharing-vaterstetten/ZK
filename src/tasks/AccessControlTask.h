#pragma once
#include "system/SystemManager.h"
#include "tasks/CardReaderTask.h"
#include "tasks/KeyControlTask.h"
#include "LedSchedulerTask.h"
#include "tasks/ModemTask.h"
#include "domain/TripTracker.h"
#include "domain/AccessStatus.h"
#include "domain/RFIDs.h"

struct ScanResult;

class AccessControlTask : public SystemThread
{
public:
    AccessControlTask(const RFIDs& rfidsManager, TripTracker& tripTracker,
                      KeyControlTask& keyControlTask, AccessStatus& accessStatus,
                      LedSchedulerTask& led, ModemTask& modem, const CardReaderTask& cardReader)
        : SystemThread(SystemThreadId::AccessControlTask, "ACCTRL", 4096, ThreadPriority::AccessControlTask, 0,
                       /*watchdogCritical=*/true),
          rfidsManager(rfidsManager),
          tripTracker(tripTracker), keyControlTask(keyControlTask), accessStatus(accessStatus), led(led), modem(modem),
          cardReader(cardReader)
    {
        SystemManager::RegisterThread(this);
    }

protected:
    void setup() override;
    void run() override;

private:
    // The card timings are interdependent; see the static_asserts below.

    /// Older than this and the scan is a leftover from a previous tap. Must clear
    /// the PN532's 200ms blocking read plus scheduling jitter, or taps get lost.
    static constexpr TickType_t scanFreshnessWindow = pdMS_TO_TICKS(500);

    /// How long the card must be gone before another tap counts. Doubles as the
    /// post-tap debounce, so a long value makes the reader feel dead.
    static constexpr TickType_t cardRemovalCooldown = pdMS_TO_TICKS(1000);

    /// A card seen this recently still counts as present. Must clear more than
    /// one reader poll so a missed beat is not read as removal.
    static constexpr TickType_t cardPresenceWindow = pdMS_TO_TICKS(500);

    /// How long a card may rest on the reader before the LED asks for it back.
    static constexpr TickType_t cardHeldGracePeriod = pdMS_TO_TICKS(1000);

    static constexpr uint32_t cardRemovalIndicatorColor = 0x3fd0d4;

    /// Fade runs from the moment removal is detectable to the moment the reader
    /// re-arms, so reaching black means "ready for the next card".
    static constexpr TickType_t cardFadeOutDuration = cardRemovalCooldown - cardPresenceWindow;

    /// Card feedback outranks ambient status output. Equal priority is FIFO and
    /// the flash is queued first, so matching the flashes keeps them in order.
    static constexpr LedPriority cardFeedbackPriority = LedPriority::High;

    /// GPS wakeup is a nice-to-have on the unlock path; dropping it when the
    /// modem is busy beats making the user wait at the car.
    static constexpr TickType_t gpsWakeupTimeToLive = pdMS_TO_TICKS(30000);

    static_assert(cardRemovalCooldown > cardPresenceWindow, "fade-out window would be empty or negative");
    static_assert(cardHeldGracePeriod > cardPresenceWindow,
                  "an already-removed card would still look present when the grace period expires");

    const RFIDs& rfidsManager;
    TripTracker& tripTracker;
    KeyControlTask& keyControlTask;
    AccessStatus& accessStatus;
    LedSchedulerTask& led;
    ModemTask& modem;
    const CardReaderTask& cardReader;

    void handleScannedCard(uint32_t rfidUid);

    /// Blocks until the card has been away for `cardRemovalCooldown`, prompting
    /// once it is clear the user has left it on the reader.
    void waitForCardRemoval();

    /// Three phases: dark while it is still unclear whether this was a tap,
    /// pulsing while the card is left on the reader, fading once it is lifted.
    static ProgressState cardRemovalFeedback(bool prompting, bool cardStillOnReader, TickType_t absentFor);
};
