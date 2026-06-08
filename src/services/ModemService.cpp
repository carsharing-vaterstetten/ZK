#include "ModemService.h"

#include <atomic>
#include <rom/rtc.h>

#include "config/Config.h"
#include "shared/Globals.h"
#include "config/user_config.h"
#include "logic/FirmwareUpdater.h"
#include "logic/HelperUtils.h"
#include "shared/RFIDs.h"
#include "modules/Modem.h"

void ModemService::OnCommand(SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        // TODO: asdf
        break;
    case SystemCommand::EnterLowPower:
        // TODO: lp
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

void ModemService::sendMessage(const ModemRxDataType dataType, const ModemFlexibleRxPayload& payload)
{
    const auto msg = new ModemRxMessage{
        .dataType = dataType,
        .payload = std::make_shared<ModemFlexibleRxPayload>(payload),
    };
    xQueueSend(modemTaskRxQueue, &msg, portMAX_DELAY);
}

void ModemService::sendMessage(const ModemTxDataType dataType, const ModemFlexibleTxPayload& payload)
{
    const auto msg = new ModemTxMessage{
        .dataType = dataType,
        .payload = std::make_shared<ModemFlexibleTxPayload>(payload),
    };
    xQueueSend(modemTaskTxQueue, &msg, portMAX_DELAY);
}

#include <chrono>

ModemTxMessage* ModemService::waitForSpecificMessage(ModemTxDataType dataType, TickType_t timeout)
{
    static std::multimap<ModemTxDataType, ModemTxMessage*> shelvedMessages;
    static std::mutex shelvedMtx;

    ModemTxMessage* receivedMsg = nullptr;

    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t deadline_ticks = start_ticks + timeout;

    while (true)
    {
        {
            std::lock_guard lock(shelvedMtx);
            auto it = shelvedMessages.find(dataType);
            if (it != shelvedMessages.end())
            {
                ModemTxMessage* match = it->second;
                shelvedMessages.erase(it);
                return match;
            }
        }

        TickType_t current_ticks = xTaskGetTickCount();
        TickType_t ticks_to_wait = 0;

        if (current_ticks - start_ticks >= timeout)

            return nullptr;

        ticks_to_wait = deadline_ticks - current_ticks;


        if (xQueueReceive(modemTaskTxQueue, &receivedMsg, ticks_to_wait) != pdTRUE)
            return nullptr;


        if (receivedMsg == nullptr || reinterpret_cast<uintptr_t>(receivedMsg) < 0x3FFA0000)
        {
            serialOnlyLog.errorln("Critical: Received a corrupted or invalid pointer from Tx Queue!");
            continue;
        }

        if (receivedMsg->dataType == dataType) return receivedMsg;

        {
            std::lock_guard lock(shelvedMtx);
            shelvedMessages.insert({receivedMsg->dataType, receivedMsg});
        }
    }
}

void ModemService::setup()
{
    modem.connect(config.simPin.c_str(), config.gprsUser.c_str(), config.gprsPassword.c_str(), config.apn.c_str());

    if (RECORD_GPS_WHILE_STANDING || (accessStatus.isLoggedIn() && accessStatus.givesGPSTrackingPermission()))
        modem.enableGPS();
}

void ModemService::run()
{
    ModemRxMessage* receivedMsg = nullptr;

    ulong lastReceivedMsgMs = millis();

    while (true)
    {
        receivedMsg = nullptr;

        xQueueReceive(modemTaskRxQueue, &receivedMsg, pdMS_TO_TICKS(5));

        if (receivedMsg == nullptr)
        {
            if (!m_running && millis() - lastReceivedMsgMs > 100)
                break;
            continue;
        }

        lastReceivedMsgMs = millis();

        switch (receivedMsg->dataType)
        {
        case ModemRxDataType::Command:
            {
                auto cmd = std::get_if<ModemTaskCommand>(receivedMsg->payload.get());

                if (cmd == nullptr)
                {
                    fileLog.debugln("Mismatched data payload inside Command type!");
                    break;
                }

                serialOnlyLog.debugln("Working on cmd " + String((uint8_t)*cmd));

                switch (*cmd)
                {
                case ModemTaskCommand::ReadData:
                    break;
                case ModemTaskCommand::WriteData:
                    break;
                case ModemTaskCommand::InitializeModem:
                    break;
                case ModemTaskCommand::PerformConnectionSpeedTest:
                    HelperUtils::performConnectionSpeedTest(api, CONNECTION_SPEED_TEST_FILE_SIZE);
                    break;
                case ModemTaskCommand::DoFirmwareUpdateIfAvailable:
                    FirmwareUpdater::doUpdateIfAvailable(api);
                    break;
                case ModemTaskCommand::DownloadRfidIfChanged:
                    rfidsManager.downloadRfidsIfChanged(api);
                    break;
                case ModemTaskCommand::DownloadGPSRfids:
                    rfidsManager.downloadGPSTrackingConsentedRFIDs(api);
                    break;
                case ModemTaskCommand::UploadLog:
                    HelperUtils::uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(api, swLog);
                    break;
                case ModemTaskCommand::DisconnectNetwork:
                    modem.disconnectNetwork();
                    break;
                case ModemTaskCommand::SleepIfPossible:
                    modem.requestSleep();
                    break;
                case ModemTaskCommand::ConnectNetwork:
                    modem.ensureNetworkConnection();
                    break;
                case ModemTaskCommand::Wakeup:
                    {
                        const bool success = modem.wakeupAndWait();
                        auto msg = new ModemTxMessage{
                            .dataType = ModemTxDataType::WakeupSuccess,
                            .payload = std::make_shared<ModemFlexibleTxPayload>(success)
                        };
                        xQueueSend(modemTaskTxQueue, &msg, portMAX_DELAY);
                        break;
                    }

                case ModemTaskCommand::EnableGPS:
                    modem.enableGPS();
                    break;
                case ModemTaskCommand::GetGPSData:
                    {
                        GPS_DATA_t data;
                        modem.getGPS(data);
                        sendMessage(ModemTxDataType::GPSData, data);
                        break;
                    }
                case ModemTaskCommand::UploadGPSData:
                    {
                        gps.uploadFileAndBeginNew(api, true, true, 2);
                        break;
                    }
                case ModemTaskCommand::GetAccessControlSequences:
                    {
                        // TODO: implement once feature on server
                        break;
                    }
                case ModemTaskCommand::GetUnixTime:
                    {
                        time_t t = modem.getUnixTimestamp();

                        for (uint i = 0; i < 100 && t < 0; ++i)
                        {
                            serialOnlyLog.debugln("Got invalid unix timestamp " + String(t));
                            vTaskDelay(pdMS_TO_TICKS(10));
                            t = modem.getUnixTimestamp();
                        }

                        if (t < 0)
                            fileLog.errorln("Failed to get unix timestamp (t=" + String(t) + ")");
                        else
                            sendMessage(ModemTxDataType::UnixTimestamp, t);

                        break;
                    }
                case ModemTaskCommand::GetTimestamp:
                    {
                        int hour, minute, second, year;

                        modem.getNetworkTime(&year, nullptr, nullptr, &hour, &minute, &second, nullptr);

                        for (uint i = 0; (year <= 0 || year > 2060) && i < 100; ++i)
                        {
                            vTaskDelay(pdMS_TO_TICKS(10));
                            modem.getNetworkTime(&year, nullptr, nullptr, &hour, &minute, &second, nullptr);
                        }

                        if (year <= 0 || year > 2060)
                        {
                            fileLog.errorln("Got invalid network time: Year " + String(year));
                        }
                        else
                        {
                            serialOnlyLog.debugln("Got time");
                            ModemTimestamp ts{
                                .hour = hour,
                                .minute = minute,
                                .second = second,
                            };

                            sendMessage(ModemTxDataType::Timestamp, ts);
                        }

                        break;
                    }
                case ModemTaskCommand::GetImei:
                    {
                        // FIXME: where does +CPIN  READY: come from?????????
                        String imei = modem.getIMEI();
                        imeiStore.setIMEI(imei);
                        serialOnlyLog.debugln("Got imei: " + imei + " | " + imeiStore.getIMEI().value_or("AAAA"));
                        break;
                    }
                }

                serialOnlyLog.debugln("Done");

                break;
            }
        }

        delete receivedMsg;
    }

    serialOnlyLog.debugln("STOP");

    SystemManager::ReportReadyForRestart(m_id);
}
