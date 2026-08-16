#include "tasks/ModemTask.h"

#include <atomic>

#include "logging/Loggers.h"
#include "config/user_config.h"
#include "net/FirmwareUpdater.h"
#include "util/HelperUtils.h"
#include "domain/RFIDs.h"
#include "hal/Modem.h"

ModemTask::~ModemTask()
{
    std::lock_guard lock(m_shelfMutex);

    for (const auto& [dataType, msg] : m_shelvedMessages)
        delete msg;

    m_shelvedMessages.clear();
}

void ModemTask::OnCommand(const SystemCommand cmd)
{
    SystemThread::OnCommand(cmd);

    switch (cmd)
    {
    case SystemCommand::EnterLowPower:
        sendRequest(ModemTaskCommand::SleepIfPossible);
        break;
    case SystemCommand::ResumeNormalOperation:
        sendRequest(ModemTaskCommand::Wakeup);
        break;
    case SystemCommand::None:
    case SystemCommand::PrepareForHotRestart:
        break;
    }
}

bool ModemTask::sendRequest(const ModemTaskCommand cmd, const TickType_t timeToLive,
                               const TickType_t enqueueTimeout)
{
    const ModemRequest request{
        .cmd = cmd,
        .deadline = timeToLive == ModemRequest::noDeadline
                        ? ModemRequest::noDeadline
                        : xTaskGetTickCount() + timeToLive,
    };

    // Counted before the send so isWorkingOnTasks() can never report idle while a
    // request is in flight between here and the run loop.
    ++m_outstandingRequests;

    if (xQueueSend(modemTaskRxQueue, &request, enqueueTimeout) == pdTRUE)
        return true;

    --m_outstandingRequests;
    logger.warningln("Modem request queue full, dropped command " + String(static_cast<int>(cmd)));
    return false;
}

void ModemTask::publish(const ModemTxDataType dataType, const ModemFlexibleTxPayload& payload)
{
    auto msg = std::make_unique<ModemTxMessage>(ModemTxMessage{
        .dataType = dataType,
        .payload = std::make_shared<ModemFlexibleTxPayload>(payload),
        .queuedAt = xTaskGetTickCount(),
    });

    ModemTxMessage* raw = msg.get();

    if (xQueueSend(modemTaskTxQueue, &raw, replyEnqueueTimeout) != pdTRUE)
    {
        // Better to lose a reply than to park the modem task on a queue that
        // nobody is draining — everything else waits behind this task.
        logger.warningln("Modem reply queue full, dropped reply " + String(static_cast<int>(dataType)));
        return;
    }

    msg.release(); // ownership passed to the receiving task
}

void ModemTask::dropStaleShelvedLocked()
{
    const TickType_t now = xTaskGetTickCount();

    for (auto it = m_shelvedMessages.begin(); it != m_shelvedMessages.end();)
    {
        if (static_cast<int32_t>(now - it->second->queuedAt) < static_cast<int32_t>(shelfLifetime))
        {
            ++it;
            continue;
        }

        delete it->second;
        it = m_shelvedMessages.erase(it);
    }
}

std::unique_ptr<ModemTxMessage> ModemTask::takeShelved(const ModemTxDataType dataType)
{
    std::lock_guard lock(m_shelfMutex);
    dropStaleShelvedLocked();

    const auto it = m_shelvedMessages.find(dataType);
    if (it == m_shelvedMessages.end()) return nullptr;

    std::unique_ptr<ModemTxMessage> msg{it->second};
    m_shelvedMessages.erase(it);
    return msg;
}

void ModemTask::shelve(std::unique_ptr<ModemTxMessage> msg)
{
    std::lock_guard lock(m_shelfMutex);
    dropStaleShelvedLocked();
    m_shelvedMessages.emplace(msg->dataType, msg.release());
}

std::unique_ptr<ModemTxMessage> ModemTask::waitForSpecificMessage(const ModemTxDataType dataType,
                                                                    const TickType_t timeout)
{
    const TickType_t deadline = xTaskGetTickCount() + timeout;

    while (true)
    {
        if (std::unique_ptr<ModemTxMessage> shelved = takeShelved(dataType)) return shelved;

        const TickType_t now = xTaskGetTickCount();
        if (static_cast<int32_t>(now - deadline) >= 0) return nullptr;

        ModemTxMessage* received = nullptr;
        if (xQueueReceive(modemTaskTxQueue, &received, deadline - now) != pdTRUE) return nullptr;
        if (received == nullptr) continue;

        std::unique_ptr<ModemTxMessage> msg{received};

        if (msg->dataType == dataType) return msg;

        shelve(std::move(msg));
    }
}

bool ModemTask::isWorkingOnTasks() const
{
    return m_outstandingRequests > 0;
}

ModemState ModemTask::getCurrentState() const
{
    return currentState;
}

void ModemTask::setup()
{
    currentState = ModemState::InitializeModem;
    modem.connect(config.simPin.c_str(), config.gprsUser.c_str(), config.gprsPassword.c_str(), config.apn.c_str());
}

void ModemTask::run()
{
    while (isRunning())
    {
        ModemRequest request{};

        if (xQueueReceive(modemTaskRxQueue, &request, idlePollInterval) != pdTRUE)
        {
            currentState = ModemState::READY;
            continue;
        }

        if (request.expired(xTaskGetTickCount()))
        {
            // The modem was busy for longer than this request was useful for.
            // Running it now would only delay whatever is queued behind it.
            serialLogger.debugln("Dropped expired modem command " + String(static_cast<int>(request.cmd)));
            --m_outstandingRequests;
            continue;
        }

        currentState = modemCmdToState(request.cmd);
        handleRequest(request.cmd);
        --m_outstandingRequests;
    }

    currentState = ModemState::NONE;
}

void ModemTask::handleRequest(const ModemTaskCommand cmd)
{
    switch (cmd)
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
        publish(ModemTxDataType::WakeupSuccess, modem.wakeupAndWait());
        break;

    case ModemTaskCommand::EnableGPS:
        modem.enableGPS();
        break;
    case ModemTaskCommand::GetGPSData:
        {
            GPS_DATA_t data;
            if (modem.getGPS(data))
                publish(ModemTxDataType::GPSData, data);
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
                serialLogger.debugln("Got invalid unix timestamp " + String(t));
                vTaskDelay(pdMS_TO_TICKS(10));
                t = modem.getUnixTimestamp();
            }

            if (t < 0)
                logger.errorln("Failed to get unix timestamp (t=" + String(t) + ")");
            else
                publish(ModemTxDataType::UnixTimestamp, t);

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
                logger.errorln("Got invalid network time: Year " + String(year));
            }
            else
            {
                serialLogger.debugln("Got time");
                ModemTimestamp ts{
                    .hour = hour,
                    .minute = minute,
                    .second = second,
                };

                publish(ModemTxDataType::Timestamp, ts);
            }

            break;
        }
    case ModemTaskCommand::GetImei:
        {
            // FIXME: where does +CPIN  READY: come from?????????
            String imei = modem.getIMEI();
            imeiStore.setIMEI(imei);
            serialLogger.debugln("Got imei: " + imei + " | " + imeiStore.getIMEI().value_or("AAAA"));
            break;
        }
    case ModemTaskCommand::NONE:
        break;
    }
}
