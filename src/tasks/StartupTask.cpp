#include "util/Time.h"
#include "StartupTask.h"

#include "config/user_config.h"
#include "system/SystemManager.h"
#include "logging/Loggers.h"

std::optional<uint> StartupTask::displayApiProgress(const ModemCommand desiredState, const uint32_t hexColor,
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
        if (timedOut() || !isRunning()) return std::nullopt;
        vTaskDelay(pollInterval);
    }

    const uint loadCmdId = led.queueLoadingCircle(hexColor);
    while (api.getState() == ApiClientState::None && xTaskGetTickCount() - start < timeoutToReachDesiredState)
    {
        if (timedOut() || !isRunning())
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

    while (isRunning() && modem.getCurrentState() == desiredState && xTaskGetTickCount() - start <
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
    modem.sendRequest(ModemCommand::GetImei);
    modem.sendRequest(ModemCommand::ConnectNetwork);
    modem.sendRequest(ModemCommand::GetUnixTime);
    modem.sendRequest(ModemCommand::GetTimestamp);

    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        modem.sendRequest(ModemCommand::PerformConnectionSpeedTest);

    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
        modem.sendRequest(ModemCommand::DoFirmwareUpdateIfAvailable);
    else
        logger.infoln("Skipped firmware update check");

    modem.sendRequest(ModemCommand::DownloadRfidIfChanged);
    modem.sendRequest(ModemCommand::DownloadGPSRfids);

    if (RECORD_GPS_WHILE_STANDING || (accessStatus.isLoggedIn() && accessStatus.givesGPSTrackingPermission()))
        modem.sendRequest(ModemCommand::EnableGPS);

    modem.sendRequest(ModemCommand::UploadLog);

    // Power saving
    modem.sendRequest(ModemCommand::DisconnectNetwork);
    modem.sendRequest(ModemCommand::SleepIfPossible);
}

void StartupTask::run()
{
    ModemCommand lastState = ModemCommand::None;
    std::optional<uint> lastLedCommandId = std::nullopt;

    while (isRunning())
    {
        ModemCommand newState = modem.getCurrentState();

        while (isRunning() && newState == lastState)
        {
            newState = modem.getCurrentState();
            vTaskDelay(pollInterval);
        }

        if (!isRunning()) break;

        if (lastLedCommandId.has_value())
            led.markCommandAsCompleted(lastLedCommandId.value());

        switch (newState)
        {
        case ModemCommand::InitializeModem:
        case ModemCommand::ConnectNetwork:
            lastLedCommandId = led.queueLoadingCircle(0xFFFFFF);
            break;
        case ModemCommand::PerformConnectionSpeedTest:
            lastLedCommandId = displayApiProgress(newState, 0x00FFFF);
            break;
        case ModemCommand::DoFirmwareUpdateIfAvailable:
            lastLedCommandId = displayApiProgress(newState, 0xFF00FF);
            break;
        case ModemCommand::DownloadRfidIfChanged:
            lastLedCommandId = displayApiProgress(newState, 0xFFA500);
            break;
        case ModemCommand::DownloadGPSRfids:
            lastLedCommandId = displayApiProgress(newState, 0xFFA500);
            break;
        case ModemCommand::UploadLog:
            lastLedCommandId = displayApiProgress(newState, 0x0000FF);
            break;
        case ModemCommand::GetUnixTime:
            {
                // Sync time
                if (const auto result = modem.waitFor(ModemResult::UnixTimestamp, pdMS_TO_TICKS(20000)))
                    Time::syncSystemTime(std::get<time_t>(*result));
                break;
            }
        default:
            break;
        }

        lastState = newState;
    }

    if (lastLedCommandId.has_value())
        led.markCommandAsCompleted(lastLedCommandId.value());
}
