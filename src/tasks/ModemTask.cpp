#include "tasks/ModemTask.h"

#include "logging/Loggers.h"
#include "config/user_config.h"
#include "net/FirmwareUpdater.h"
#include "util/HelperUtils.h"
#include "domain/RFIDs.h"
#include "hal/Modem.h"

void ModemTask::OnCommand(const SystemCommand cmd)
{
    SystemThread::OnCommand(cmd);

    switch (cmd)
    {
    case SystemCommand::EnterLowPower:
        sendRequest(ModemCommand::SleepIfPossible);
        break;
    case SystemCommand::ResumeNormalOperation:
        sendRequest(ModemCommand::Wakeup);
        break;
    case SystemCommand::None:
    case SystemCommand::PrepareForHotRestart:
        break;
    }
}

bool ModemTask::sendRequest(const ModemCommand cmd, const TickType_t timeToLive, const TickType_t enqueueTimeout)
{
    const ModemRequest request{
        .cmd = cmd,
        .deadline = timeToLive == ModemRequest::noDeadline
                        ? ModemRequest::noDeadline
                        : xTaskGetTickCount() + timeToLive,
    };

    // Counted before the send so hasPendingWork() can never report idle while a
    // request is in flight between here and the run loop.
    ++m_outstandingRequests;

    if (xQueueSend(m_requests, &request, enqueueTimeout) == pdTRUE)
        return true;

    --m_outstandingRequests;
    logger.warningln("Modem request queue full, dropped command " + String(static_cast<int>(cmd)));
    return false;
}

void ModemTask::publish(const ModemResult type, ModemPayload payload)
{
    {
        std::lock_guard lock(m_resultsMutex);
        m_results[static_cast<size_t>(type)] = std::move(payload);
    }

    xEventGroupSetBits(m_resultReady, bitFor(type));
}

std::optional<ModemPayload> ModemTask::take(const ModemResult type)
{
    std::lock_guard lock(m_resultsMutex);

    std::optional<ModemPayload>& slot = m_results[static_cast<size_t>(type)];
    if (!slot.has_value()) return std::nullopt;

    std::optional<ModemPayload> value = std::move(slot);
    slot.reset();

    // Cleared here rather than on wait exit, so a leftover bit from a result that
    // has already been consumed cannot cut a later wait short.
    xEventGroupClearBits(m_resultReady, bitFor(type));

    return value;
}

std::optional<ModemPayload> ModemTask::waitFor(const ModemResult type, const TickType_t timeout)
{
    const TickType_t deadline = xTaskGetTickCount() + timeout;

    while (true)
    {
        // Checked before waiting: a result published before the caller got here
        // is still the answer to its question.
        if (std::optional<ModemPayload> value = take(type)) return value;

        const TickType_t now = xTaskGetTickCount();
        if (static_cast<int32_t>(now - deadline) >= 0) return std::nullopt;

        xEventGroupWaitBits(m_resultReady, bitFor(type), pdFALSE, pdTRUE, deadline - now);
    }
}

void ModemTask::setup()
{
    m_currentState = ModemCommand::InitializeModem;
    modem.connect(config.simPin.c_str(), config.gprsUser.c_str(), config.gprsPassword.c_str(), config.apn.c_str());
}

void ModemTask::run()
{
    while (isRunning())
    {
        ModemRequest request{};

        if (xQueueReceive(m_requests, &request, idlePollInterval) != pdTRUE)
        {
            m_currentState = ModemCommand::Ready;
            continue;
        }

        if (request.expired(xTaskGetTickCount()))
        {
            // The modem was busy for longer than this request was useful for;
            // running it now would only delay whatever is queued behind it.
            serialLogger.debugln("Dropped expired modem command " + String(static_cast<int>(request.cmd)));
            --m_outstandingRequests;
            continue;
        }

        m_currentState = request.cmd;
        handleRequest(request.cmd);
        --m_outstandingRequests;
    }

    m_currentState = ModemCommand::None;
}

void ModemTask::publishUnixTime()
{
    time_t t = modem.getUnixTimestamp();

    for (uint i = 0; i < 100 && t < 0; ++i)
    {
        serialLogger.debugln("Got invalid unix timestamp " + String(t));
        vTaskDelay(pdMS_TO_TICKS(10));
        t = modem.getUnixTimestamp();
    }

    if (t < 0)
    {
        logger.errorln("Failed to get unix timestamp (t=" + String(t) + ")");
        return;
    }

    publish(ModemResult::UnixTimestamp, t);
}

void ModemTask::publishNetworkTime()
{
    int hour = 0, minute = 0, second = 0, year = 0;

    modem.getNetworkTime(&year, nullptr, nullptr, &hour, &minute, &second, nullptr);

    for (uint i = 0; (year <= 0 || year > 2060) && i < 100; ++i)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        modem.getNetworkTime(&year, nullptr, nullptr, &hour, &minute, &second, nullptr);
    }

    if (year <= 0 || year > 2060)
    {
        logger.errorln("Got invalid network time: Year " + String(year));
        return;
    }

    serialLogger.debugln("Got time");
    publish(ModemResult::Timestamp, ModemTimestamp{.hour = hour, .minute = minute, .second = second});
}

void ModemTask::handleRequest(const ModemCommand cmd)
{
    switch (cmd)
    {
    case ModemCommand::PerformConnectionSpeedTest:
        HelperUtils::performConnectionSpeedTest(api, CONNECTION_SPEED_TEST_FILE_SIZE);
        break;
    case ModemCommand::DoFirmwareUpdateIfAvailable:
        FirmwareUpdater::doUpdateIfAvailable(api);
        break;
    case ModemCommand::DownloadRfidIfChanged:
        rfidsManager.downloadRfidsIfChanged(api);
        break;
    case ModemCommand::DownloadGPSRfids:
        rfidsManager.downloadGPSTrackingConsentedRFIDs(api);
        break;
    case ModemCommand::UploadLog:
        HelperUtils::uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(api, swLog);
        break;
    case ModemCommand::DisconnectNetwork:
        modem.disconnectNetwork();
        break;
    case ModemCommand::SleepIfPossible:
        modem.requestSleep();
        break;
    case ModemCommand::ConnectNetwork:
        modem.ensureNetworkConnection();
        break;
    case ModemCommand::Wakeup:
        publish(ModemResult::WakeupSuccess, modem.wakeupAndWait());
        break;
    case ModemCommand::EnableGPS:
        modem.enableGPS();
        break;
    case ModemCommand::GetGPSData:
        {
            GPS_DATA_t data;
            if (modem.getGPS(data)) publish(ModemResult::GPSData, data);
            break;
        }
    case ModemCommand::UploadGPSData:
        gps.uploadFileAndBeginNew(api, true, true, 2);
        break;
    case ModemCommand::GetUnixTime:
        publishUnixTime();
        break;
    case ModemCommand::GetTimestamp:
        publishNetworkTime();
        break;
    case ModemCommand::GetImei:
        // FIXME: where does +CPIN  READY: come from?????????
        imeiStore.setIMEI(modem.getIMEI());
        break;
    case ModemCommand::GetAccessControlSequences: // TODO: implement once feature on server
    case ModemCommand::InitializeModem: // handled once in setup()
    case ModemCommand::None:
    case ModemCommand::Ready:
        break;
    }
}
