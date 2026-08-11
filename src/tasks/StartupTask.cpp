#include "StartupTask.h"

#include "config/user_config.h"
#include "logic/HelperUtils.h"
#include "logic/SystemManager.h"
#include "shared/Globals.h"

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
        if (timedOut()) return std::nullopt;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    const uint loadCmdId = led.queueLoadingCircle(hexColor);
    while (api.getState() == ApiClientState::None && xTaskGetTickCount() - start < timeoutToReachDesiredState)
    {
        if (timedOut())
        {
            led.markCommandAsCompleted(loadCmdId);
            return std::nullopt;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    led.markCommandAsCompleted(loadCmdId);

    ProgressState state{0, hexColor};
    const uint cmdId = led.queueProgressIndicator();

    start = xTaskGetTickCount();

    while (modem.getCurrentState() == desiredState && xTaskGetTickCount() - start < timeToCompleteDesiredState)
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

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return cmdId;
}

void StartupTask::setup()
{
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


    ModemState lastState = ModemState::NONE;
    std::optional<uint> lastLedCommandId = std::nullopt;
    while (true)
    {
        ModemState newState = modem.getCurrentState();

        while (newState == lastState)
        {
            newState = modem.getCurrentState();
            vTaskDelay(pdMS_TO_TICKS(10));
        }

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
                ModemTxMessage* unixTimestamp = modem.waitForSpecificMessage(
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
}


void StartupTask::run()
{
    SystemManager::ReportReadyForRestart(m_id);
}
