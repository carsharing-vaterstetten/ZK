#include "AccessControlTask.h"

#include "tasks/CardReaderTask.h"
#include "logging/Loggers.h"

void AccessControlTask::handleScannedCard(const uint32_t rfidUid)
{
    if (!rfidsManager.isRegisteredRFID(rfidUid))
    {
        logger.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        led.queueCardDeclinedFlash();
        return;
    }

    logger.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");

    if (!accessStatus.isLoggedIn())
    {
        // Don't signal success for a key command that never made it to the key
        // task — the LED is the only thing telling the user whether the car
        // actually reacted. (This still only proves it was queued; see note in
        // KeyControlTask about confirming the sequence itself.)
        if (!keyControlTask.unlock())
        {
            logger.errorln("Failed to queue unlock");
            led.queueCardDeclinedFlash();
            return;
        }

        led.queueUnlockFlash();
        accessStatus.setLoginData(rfidUid);

        logger.infoln("Car unlocked");

        if (rfidsManager.RFIDConsentsToGPSTrackingTest(rfidUid))
        {
            modem.sendRequest(ModemCommand::Wakeup, gpsWakeupTimeToLive);
            modem.sendRequest(ModemCommand::EnableGPS, gpsWakeupTimeToLive);

            if (tripTracker.startTrip())
                logger.infoln("Trip started");
        }
    }
    else
    {
        if (!keyControlTask.lock())
        {
            logger.errorln("Failed to queue lock");
            led.queueCardDeclinedFlash();
            return;
        }

        led.queueLockFlash();
        accessStatus.clrLoginData();

        logger.infoln("Car locked");

        if (const std::optional<float> traveledDistance = tripTracker.endTrip())
            logger.infoln("Trip ended. Traveled distance: " + String(*traveledDistance) + " m");
    }
}

ProgressState AccessControlTask::cardRemovalFeedback(const bool prompting, const bool cardStillOnReader,
                                                     const TickType_t absentFor)
{
    // Claimed but with nothing to say yet: still deciding whether this was a tap
    // or a card left behind.
    if (!prompting)
        return ProgressState{.progress = 0.0f, .colorHex = cardRemovalIndicatorColor, .effect = LedEffect::Fade};

    // Card is still sitting there. Pulsing is an instruction to the user, and it
    // has to look unlike the boot sequence's bars, which mean "wait".
    if (cardStillOnReader)
        return ProgressState{.progress = 0.0f, .colorHex = cardRemovalIndicatorColor, .effect = LedEffect::Pulse};

    // Card is off and the user has nothing left to do, so the light just goes
    // away. Measured from the moment removal was detected rather than from the
    // card's last sighting, so the fade starts at full instead of jumping to
    // whatever fraction of the cooldown had already elapsed.
    const TickType_t fadedFor = absentFor - cardPresenceWindow;

    return ProgressState{
        .progress = 1.0f - static_cast<float>(fadedFor) / cardFadeOutDuration,
        .colorHex = cardRemovalIndicatorColor,
        .effect = LedEffect::Fade,
    };
}

void AccessControlTask::waitForCardRemoval()
{
    constexpr TickType_t pollInterval = pdMS_TO_TICKS(50);

    const TickType_t enteredAt = xTaskGetTickCount();
    TickType_t lastSeen = enteredAt;

    // Claimed up front, at the same priority as the lock/unlock flashes, so that
    // for as long as a card is on the reader the strip belongs to access control
    // and nothing else — a startup progress bar, a loading circle — can take it
    // over. It renders dark until there is actually something to say, which is
    // why it can be claimed this early without giving a plain tap a cyan tail.
    const uint progressId = led.queueProgressIndicator(cardFeedbackPriority);

    bool countdownVisible = false;

    while (isRunning())
    {
        if (const std::optional<ScanResult> scan = cardReader.waitForScanResult(pollInterval))
            lastSeen = scan->ts;

        const TickType_t now = xTaskGetTickCount();
        const TickType_t absentFor = now - lastSeen;

        if (absentFor >= cardRemovalCooldown) break;

        // Both conditions matter. The grace period alone is not enough: after a
        // normal tap the card is already gone, and asking for it back would just
        // light the LED at someone holding nothing.
        const bool cardStillOnReader = absentFor < cardPresenceWindow;
        const bool heldPastGracePeriod = now - enteredAt >= cardHeldGracePeriod;

        // Latched: once the card is confirmed to be resting on the reader we keep
        // showing feedback, so the user sees the result of lifting it off rather
        // than having the LED go blank the instant the reader loses contact.
        countdownVisible = countdownVisible || (cardStillOnReader && heldPastGracePeriod);

        led.updateProgressOfCommand(progressId, cardRemovalFeedback(countdownVisible, cardStillOnReader, absentFor));
    }

    led.markCommandAsCompleted(progressId);
}

void AccessControlTask::setup()
{
    logger.debugln("Card scanner task started");
}

void AccessControlTask::run()
{
    while (isRunning())
    {
        const std::optional<ScanResult> scan = cardReader.waitForScanResult(pdMS_TO_TICKS(500));
        if (!scan.has_value()) continue;

        if (xTaskGetTickCount() - scan->ts > scanFreshnessWindow) continue;

        handleScannedCard(scan->uid);

        waitForCardRemoval();
    }

    logger.debugln("Card scanner task ended");
}
