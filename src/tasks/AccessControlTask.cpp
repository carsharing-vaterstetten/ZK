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

void AccessControlTask::waitForCardRemoval()
{
    constexpr TickType_t pollInterval = pdMS_TO_TICKS(50);

    const TickType_t enteredAt = xTaskGetTickCount();
    TickType_t lastSeen = enteredAt;

    std::optional<uint> progressId;

    while (m_running)
    {
        if (const std::optional<ScanResult> scan = cardReader.waitForScanResult(pollInterval))
            lastSeen = scan->ts;

        const TickType_t now = xTaskGetTickCount();
        const TickType_t absentFor = now - lastSeen;

        if (absentFor >= cardRemovalCooldown) break;

        // Only start asking for the card back once the user has clearly left it
        // on the reader — a normal tap is over long before this.
        if (!progressId.has_value() && now - enteredAt >= cardHeldGracePeriod)
            progressId = led.queueProgressIndicator();

        if (progressId.has_value())
        {
            const ProgressState state{
                .progress = static_cast<float>(absentFor) / cardRemovalCooldown,
                .colorHex = cardRemovalIndicatorColor,
            };
            led.updateProgressOfCommand(progressId.value(), state);
        }
    }

    if (progressId.has_value())
        led.markCommandAsCompleted(progressId.value());
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
