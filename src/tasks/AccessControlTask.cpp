#include "AccessControlTask.h"

#include "services/CardReaderService.h"
#include "shared/Globals.h"
#include "shared/SharedData.h"

void AccessControlTask::doThings(const uint32_t rfidUid)
{
    if (!rfidsManager.isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        led.playSequence(LedSequence::CardDeclined);
        return;
    }

    fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");

    if (!accessStatus.isLoggedIn())
    {
        keyControlService.unlock();
        led.playSequence(LedSequence::CarUnlocked);
        accessStatus.setLoginData(rfidUid);

        fileLog.infoln("Car unlocked");

        if (rfidsManager.RFIDConsentsToGPSTrackingTest(rfidUid))
        {
            modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::Wakeup);
            modem.sendMessage(ModemRxDataType::Command, ModemTaskCommand::EnableGPS);

            if (!gpsAlg.isTripActive())
            {
                gpsAlg.startTrip();
                fileLog.infoln("Trip started");
            }
        }
    }
    else
    {
        keyControlService.lock();
        led.playSequence(LedSequence::CarLocked);
        accessStatus.clrLoginData();

        fileLog.infoln("Car locked");

        if (gpsAlg.isTripActive())
        {
            const float traveledDistance = gpsAlg.endTrip();
            fileLog.infoln("Trip ended. Traveled distance: " + String(traveledDistance) + " m");
        }
    }
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
    constexpr TickType_t cooldown1 = pdMS_TO_TICKS(1000);

    while (m_running)
    {
        std::optional<ScanResult> result = cardReader.waitForScanResult(pdMS_TO_TICKS(500));
        if (!result.has_value()) continue;

        // make sure it's an up-to-date scan
        if (xTaskGetTickCount() - result->ts > pdMS_TO_TICKS(10)) continue;

        doThings(result->uid);

        vTaskDelay(cooldown1);

        result = cardReader.waitForScanResult(pdMS_TO_TICKS(10));

        if (!result.has_value()) continue; // no card scanned while cooldown1

        const TickType_t resultAge = xTaskGetTickCount() - result->ts;
        if (resultAge > cooldown1 / 10) continue; // card was removed during cooldown1

        led.playSequence(LedSequence::WaitingForCardRemoval);
        vTaskDelay(LedService::cardRemovalCooldown);
    }

    fileLog.debugln("Card scanner task ended");

    SystemManager::ReportReadyForRestart(m_id);
}
