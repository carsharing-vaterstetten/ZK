#include "ModemService.h"

#include <atomic>

#include "shared/Globals.h"
#include "config/user_config.h"
#include "logic/FirmwareUpdater.h"
#include "logic/HelperUtils.h"
#include "shared/RFIDs.h"
#include "modules/Modem.h"

void ModemService::OnCommand(const SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        break;
    case SystemCommand::EnterLowPower:
        lowPowerRequested = true;
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

void ModemService::sendRequest(const ModemTaskCommand cmd) const
{
    xQueueSend(modemTaskRxQueue, &cmd, portMAX_DELAY);
}

void ModemService::sendAnswer(const ModemTxDataType dataType, const ModemFlexibleTxPayload& payload) const
{
    const auto msg = new ModemTxMessage{
        .dataType = dataType,
        .payload = std::make_shared<ModemFlexibleTxPayload>(payload),
    };
    xQueueSend(modemTaskTxQueue, &msg, portMAX_DELAY);
}

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

bool ModemService::isWorkingOnTasks()
{
    return workingOnTasks;
}

ModemState ModemService::getCurrentState()
{
    return currentSate;
}

void ModemService::setup()
{
    currentSate = ModemState::InitializeModem;
    modem.connect(config.simPin.c_str(), config.gprsUser.c_str(), config.gprsPassword.c_str(), config.apn.c_str());
}

void ModemService::run()
{
    ModemTaskCommand receivedCmd = ModemTaskCommand::NONE;

    while (true)
    {
        currentSate = ModemState::READY;
        receivedCmd = ModemTaskCommand::NONE;
        xQueueReceive(modemTaskRxQueue, &receivedCmd, pdMS_TO_TICKS(5));

        if (receivedCmd == ModemTaskCommand::NONE)
        {
            if (xTaskGetTickCount() - lastReceivedMessageTime < pdMS_TO_TICKS(100)) continue;

            workingOnTasks = false;

            if (!m_running) break;

            continue;
        }

        workingOnTasks = true;
        currentSate = modemCmdToState(receivedCmd);
        lastReceivedMessageTime = xTaskGetTickCount();

        // serialOnlyLog.debugln("Working on cmd " + String((int)receivedCmd));

        switch (receivedCmd)
        {
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
                sendAnswer(ModemTxDataType::GPSData, data);
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
                    sendAnswer(ModemTxDataType::UnixTimestamp, t);

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

                    sendAnswer(ModemTxDataType::Timestamp, ts);
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
        case ModemTaskCommand::NONE:
            break;
        }
    }

    workingOnTasks = false;
    currentSate = ModemState::NONE;

    SystemManager::ReportReadyForRestart(m_id);
}
