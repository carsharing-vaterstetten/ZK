#include "ModemService.h"

#include <atomic>
#include <LittleFS.h>
#include <rom/rtc.h>

#include "Config.h"
#include "shared/Globals.h"
#include "TinyGsmClientSIM7000.h"
#include "config/Backend.h"
#include "drivers/modem.h"
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

void ModemService::queueModemTaskRxJob(const ModemRxDataType dataType, ModemFlexibleRxPayload payload)
{
    const auto msg = new ModemRxMessage{
        .dataType = dataType,
        .payload = std::make_shared<ModemFlexibleRxPayload>(payload),
    };
    xQueueSend(modemTaskRxQueue, &msg, portMAX_DELAY);
}

ModemTxMessage* ModemService::waitForSpecificModemMessage(ModemTxDataType dataType)
{
    static std::multimap<ModemTxDataType, ModemTxMessage*> shelvedMessages;
    static std::mutex shelvedMtx;

    ModemTxMessage* receivedMsg = nullptr;

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

        xQueueReceive(modemTaskTxQueue, &receivedMsg, portMAX_DELAY);

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

void modemTask(void* pvParams) {}

void ModemService::setup()
{
    // Allocate heavy objects on the HEAP instead of the STACK
    // Modules
    constexpr auto gpsFilePath = "/gps.bin";
    GPS gps{gpsFilePath, GPS_FILE_UPLOAD_ENDPOINT};
    auto gsmModem = std::make_unique<TinyGsmSim7000>(Serial1);
    const ModemDriver modemDriver{board};
    auto modem = std::make_unique<Modem>(*gsmModem, Serial1, MODEM_SERIAL_BAUD, modemDriver);

    LittleFS.remove(gpsFilePath); // Delete old file

    modem->begin(config.simPin.c_str(), config.gprsUser.c_str(), config.gprsPassword.c_str(), config.apn.c_str());
    gps.begin();

    // In my tests connecting network first, then gprs is best after a hard reset (e.g. code upload). The other order after a ESP.restart().
    modem->ensureNetworkConnection(rtc_get_reset_reason(0) != POWERON_RESET);
    HelperUtils::syncTimeWithModem(*modem, 20);

    if (RECORD_GPS_WHILE_STANDING || (accessStatus.isLoggedIn() && accessStatus.givesGPSTrackingPermission()))
        modem->enableGPS();

    fileLog.infoln("Signal Quality: " + String(modem->getSignalQuality()));
    fileLog.infoln(
        "Time (v1.0.0): millis: " + String(millis()) + " ms, Localtime: " + modem->getGSMDateTime() +
        ", Unix timestamp: " + String(modem->getUnixTimestamp()) + ", system time: " + String(
            HelperUtils::systemTimeMillisecondsSinceEpoche()) + " ms");

    String modemIMEI = modem->getIMEI();

    fileLog.infoln("Modem IMEI: " + modemIMEI);

    int hour, minute, second;
    modem->getNetworkTime(nullptr, nullptr, nullptr, &hour, &minute, &second, nullptr);
    ModemTimestamp ts{
        .hour = hour,
        .minute = minute,
        .second = second,
    };

    const ModemTxMessage* timestampMessage = new ModemTxMessage{
        .dataType = ModemTxDataType::Timestamp,
        .payload = std::make_shared<ModemFlexibleTxPayload>(ts),
    };

    xQueueSend(modemTaskTxQueue, &timestampMessage, portMAX_DELAY);

    auto gsmClient = TinyGsmSim7000::GsmClientSim7000{*gsmModem};
    auto modemClient = WdClient{gsmClient, config.server, config.serverPort};
    api.emplace(modemClient, modemIMEI, config.serverPassword);
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

                switch (*cmd)
                {
                case ModemTaskCommand::ReadData:
                    break;
                case ModemTaskCommand::WriteData:
                    break;
                case ModemTaskCommand::InitializeModem:
                    break;
                case ModemTaskCommand::PerformConnectionSpeedTest:
                    HelperUtils::performConnectionSpeedTest(*api, CONNECTION_SPEED_TEST_FILE_SIZE);
                    break;
                case ModemTaskCommand::DoFirmwareUpdateIfAvailable:
                    FirmwareUpdater::doUpdateIfAvailable(*api);
                    break;
                case ModemTaskCommand::DownloadRfidIfChanged:
                    rfidsManager.downloadRfidsIfChanged(*api);
                    break;
                case ModemTaskCommand::DownloadGPSRfids:
                    rfidsManager.downloadGPSTrackingConsentedRFIDs(*api);
                    break;
                case ModemTaskCommand::UploadLog:
                    HelperUtils::uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(*api, swLog);
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
                        gps.writeData(data);
                        auto msg = new ModemTxMessage{
                            .dataType = ModemTxDataType::GPSData,
                            .payload = std::make_shared<ModemFlexibleTxPayload>(data)
                        };
                        xQueueSend(modemTaskTxQueue, &msg, portMAX_DELAY);
                        break;
                    }
                case ModemTaskCommand::UploadGPSData:
                    {
                        gps.uploadFileAndBeginNew(*api, true, true, 2);
                        break;
                    }
                case ModemTaskCommand::GetAccessControlSequences:
                    {
                        // TODO: implement once feature on server
                        break;
                    }
                }

                break;
            }
        }

        delete receivedMsg;
    }

    SystemManager::ReportReadyForRestart(m_id);
}
