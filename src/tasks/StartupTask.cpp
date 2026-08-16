#include "StartupTask.h"

#include "config/user_config.h"
#include "util/HelperUtils.h"
#include "system/SystemManager.h"
#include "logging/Loggers.h"

void StartupTask::OnCommand(const SystemCommand cmd)
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

std::optional<uint> StartupTask::displayApiProgress(const ModemState desiredState, const uint32_t hexColor,
                                                    TickType_t timeoutToReachDesiredState = pdMS_TO_TICKS(5000),
                                                    const TickType_t timeToCompleteDesiredState = pdMS_TO_TICKS(60 * 1000))
{
    TickType_t start = xTaskGetTickCount();

    auto timedOut = [start, timeoutToReachDesiredState]
    {
        return timeoutToReachDesiredState < xTaskGetTickCount() - start;
    };

    while (modem.getCurrentState() != desiredState)
    {
        if (timedOut() || !m_running) return std::nullopt;
        vTaskDelay(pollInterval);
    }

    const uint loadCmdId = led.queueLoadingCircle(hexColor);
    while (api.getState() == ApiClientState::None && xTaskGetTickCount() - start < timeoutToReachDesiredState)
    {
        if (timedOut() || !m_running)
        {
            led.markCommandAsCompleted(loadCmdId);
            return std::nullopt;
        }
        vTaskDelay(pollInterval);
    }
    led.markCommandAsCompleted(loadCmdId);

    ProgressState state{0, hexColor};
    const uint cmdId = led.queueProgressIndicator();

    start = xTaskGetTickCount();

    while (m_running && modem.getCurrentState() == desiredState && xTaskGetTickCount() - start <
        timeToCompleteDesiredState)
    {
        auto [bytesProcessed, bytesTotal] = api.getProgress();

        float progress = 0;
        if (bytesTotal != 0)
            progress = static_cast<float>(bytesProcessed) / bytesTotal;

        switch (api.getState())
        {
        case ApiClientState::None:
            break;
        case ApiClientState::Downloading:
            state.progress = 1.0f - progress;
            led.updateProgressOfCommand(cmdId, state);
            break;
        case ApiClientState::Uploading:
            state.progress = progress;
            led.updateProgressOfCommand(cmdId, state);
            break;
        }

        vTaskDelay(pollInterval);
    }

    return cmdId;
}

void StartupTask::setup()
{
    // The boot sequence must complete even if the modem is slow, so none of these
    // carry a deadline; they are queued once, before anything else can compete
    // for the modem.
    modem.sendRequest(ModemTaskCommand::GetImei);
    modem.sendRequest(ModemTaskCommand::ConnectNetwork);
    modem.sendRequest(ModemTaskCommand::GetUnixTime);
    modem.sendRequest(ModemTaskCommand::GetTimestamp);

    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        modem.sendRequest(ModemTaskCommand::PerformConnectionSpeedTest);

    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
        modem.sendRequest(ModemTaskCommand::DoFirmwareUpdateIfAvailable);
    else
        fileLog.infoln("Skipped firmware update check");

    modem.sendRequest(ModemTaskCommand::DownloadRfidIfChanged);
    modem.sendRequest(ModemTaskCommand::DownloadGPSRfids);

    if (RECORD_GPS_WHILE_STANDING || (accessStatus.isLoggedIn() && accessStatus.givesGPSTrackingPermission()))
        modem.sendRequest(ModemTaskCommand::EnableGPS);

    modem.sendRequest(ModemTaskCommand::UploadLog);

    // Power saving
    modem.sendRequest(ModemTaskCommand::DisconnectNetwork);
    modem.sendRequest(ModemTaskCommand::SleepIfPossible);
}

void StartupTask::run()
{
    ModemState lastState = ModemState::NONE;
    std::optional<uint> lastLedCommandId = std::nullopt;

    while (m_running)
    {
        ModemState newState = modem.getCurrentState();

        while (m_running && newState == lastState)
        {
            newState = modem.getCurrentState();
            vTaskDelay(pollInterval);
        }

        if (!m_running) break;

        if (lastLedCommandId.has_value())
            led.markCommandAsCompleted(lastLedCommandId.value());

        switch (newState)
        {
        case ModemState::InitializeModem:
        case ModemState::ConnectNetwork:
            lastLedCommandId = led.queueLoadingCircle(0xFFFFFF);
            break;
        case ModemState::PerformConnectionSpeedTest:
            lastLedCommandId = displayApiProgress(newState, 0x00FFFF);
            break;
        case ModemState::DoFirmwareUpdateIfAvailable:
            lastLedCommandId = displayApiProgress(newState, 0xFF00FF);
            break;
        case ModemState::DownloadRfidIfChanged:
            lastLedCommandId = displayApiProgress(newState, 0xFFA500);
            break;
        case ModemState::DownloadGPSRfids:
            lastLedCommandId = displayApiProgress(newState, 0xFFA500);
            break;
        case ModemState::UploadLog:
            lastLedCommandId = displayApiProgress(newState, 0x0000FF);
            break;
        case ModemState::GetUnixTime:
            {
                // Sync time
                const std::unique_ptr<ModemTxMessage> unixTimestamp = modem.waitForSpecificMessage(
                    ModemTxDataType::UnixTimestamp, pdMS_TO_TICKS(20000));
                if (unixTimestamp != nullptr)
                    HelperUtils::syncSystemTime(std::get<time_t>(*unixTimestamp->payload));
                break;
            }
        default:
            break;
        }

        lastState = newState;
    }

    if (lastLedCommandId.has_value())
        led.markCommandAsCompleted(lastLedCommandId.value());

    SystemManager::ReportReadyForRestart(m_id);
}
