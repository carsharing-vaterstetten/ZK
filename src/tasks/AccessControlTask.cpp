#include "AccessControlTask.h"

#include "services/CardReaderService.h"
#include "shared/Globals.h"
#include "shared/SharedData.h"

ScanResult AccessControlTask::scan(const TickType_t timeout)
{
    return *static_cast<ScanResult*>(SharedData::receiveDataFromCommand(CardReaderService::newCardScannedId, timeout));
}

void AccessControlTask::doThings(const uint32_t rfidUid)
{
    if (!rfidsManager.isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        led.playSequence(LedSequence::CardDeclined);
        return;
    }

    fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");

    const bool login = !accessStatus.isLoggedIn();

    keyControlService.toggleLogin(rfidUid);

    if (login)
    {
        led.playSequence(LedSequence::CarUnlocked);

        if (rfidsManager.RFIDConsentsToGPSTrackingTest(rfidUid))
        {
            modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::Wakeup);
            modem.queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::EnableGPS);

            if (!gpsAlg.isTripActive())
            {
                gpsAlg.startTrip();
                fileLog.infoln("Trip started");
            }
        }
    }
    else
    {
        led.playSequence(LedSequence::CarLocked);

        if (gpsAlg.isTripActive())
        {
            const float traveledDistance = gpsAlg.endTrip();
            fileLog.infoln("Trip ended. Traveled distance: " + String(traveledDistance) + " m");
        }
    }
}

void AccessControlTask::cooldownSequence() const
{
    const ulong firstScanMs = millis();

    // Wait for 2 seconds for the card to be removed
    constexpr uint waitForRemovalMs = 2000;
    vTaskDelay(pdMS_TO_TICKS(waitForRemovalMs));

    // Then check again for 1 second if a card is present
    constexpr uint waitForScanMs = 1000;

    ScanResult scanResult = scan(pdMS_TO_TICKS(100));
    while (millis() - firstScanMs < waitForRemovalMs + waitForScanMs)
        scanResult = scan(pdMS_TO_TICKS(100));

    if (scanResult.status != ScanStatus::Duplicate) return;

    // If it scanned the same card twice wait another 3 seconds for it to be removed
    // and indicate a cooldown via the LED

    led.playSequence(LedSequence::WaitingForCardRemoval);
    vTaskDelay(pdMS_TO_TICKS(3000));

    // From the first card scan to here it should be 6 seconds
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
        auto [status, uid] = scan(portMAX_DELAY);

        switch (status)
        {
        case ScanStatus::NoCard:
            break;
        case ScanStatus::NewCard:
            doThings(uid);
            cooldownSequence();
            break;
        case ScanStatus::Duplicate:
            break;
        }
    }

    fileLog.debugln("Card scanner task ended");

    SystemManager::ReportReadyForRestart(m_id);

    vTaskDelete(nullptr);
}
