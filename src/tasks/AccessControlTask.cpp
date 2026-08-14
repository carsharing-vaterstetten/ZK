#include "AccessControlTask.h"

#include "services/CardReaderService.h"
#include "shared/Globals.h"
#include "shared/SharedData.h"

void AccessControlTask::doThings(const uint32_t rfidUid)
{
    if (!rfidsManager.isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        led.queueCardDeclinedFlash();
        return;
    }

    fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");

    if (!accessStatus.isLoggedIn())
    {
        // Don't signal success for a key command that never made it to the key
        // task — the LED is the only thing telling the user whether the car
        // actually reacted. (This still only proves it was queued; see note in
        // KeyControlService about confirming the sequence itself.)
        if (!keyControlService.unlock())
        {
            fileLog.errorln("Failed to queue unlock");
            led.queueCardDeclinedFlash();
            return;
        }

        led.queueUnlockFlash();
        accessStatus.setLoginData(rfidUid);

        fileLog.infoln("Car unlocked");

        if (rfidsManager.RFIDConsentsToGPSTrackingTest(rfidUid))
        {
            modem.sendRequest(ModemTaskCommand::Wakeup, gpsWakeupTimeToLive);
            modem.sendRequest(ModemTaskCommand::EnableGPS, gpsWakeupTimeToLive);

            if (!gpsAlg.isTripActive())
            {
                gpsAlg.startTrip();
                fileLog.infoln("Trip started");
            }
        }
    }
    else
    {
        if (!keyControlService.lock())
        {
            fileLog.errorln("Failed to queue lock");
            led.queueCardDeclinedFlash();
            return;
        }

        led.queueLockFlash();
        accessStatus.clrLoginData();

        fileLog.infoln("Car locked");

        if (gpsAlg.isTripActive())
        {
            const float traveledDistance = gpsAlg.endTrip();
            fileLog.infoln("Trip ended. Traveled distance: " + String(traveledDistance) + " m");
        }
    }
}

ProgressState AccessControlTask::cardRemovalFeedback(const bool prompting, const bool cardStillOnReader,
                                                     const TickType_t absentFor)
{
    // Claimed but with nothing to say yet: still deciding whether this was a tap
    // or a card left behind. An empty bar renders dark.
    if (!prompting)
        return ProgressState{.progress = 0.0f, .colorHex = cardRemovalIndicatorColor, .effect = LedEffect::Bar};

    // Card is still sitting there. Pulsing is an instruction to the user, and it
    // has to look unlike the boot sequence's bars, which mean "wait".
    if (cardStillOnReader)
        return ProgressState{.progress = 0.0f, .colorHex = cardRemovalIndicatorColor, .effect = LedEffect::Pulse};

    // Card is off. Drain the bar to show the reader coming back up, so the dead
    // time reads as "nearly ready" instead of as nothing happening.
    return ProgressState{
        .progress = 1.0f - static_cast<float>(absentFor) / cardRemovalCooldown,
        .colorHex = cardRemovalIndicatorColor,
        .effect = LedEffect::Bar,
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

    while (m_running)
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

void AccessControlTask::OnCommand(SystemCommand cmd)
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

void AccessControlTask::setup()
{
    fileLog.debugln("Card scanner task started");
}

void AccessControlTask::run()
{
    while (m_running)
    {
        const std::optional<ScanResult> scan = cardReader.waitForScanResult(pdMS_TO_TICKS(500));
        if (!scan.has_value()) continue;

        if (xTaskGetTickCount() - scan->ts > scanFreshnessWindow) continue;

        doThings(scan->uid);

        waitForCardRemoval();
    }

    fileLog.debugln("Card scanner task ended");

    SystemManager::ReportReadyForRestart(m_id);
}
