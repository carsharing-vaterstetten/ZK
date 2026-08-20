#include "util/Time.h"
#include "StartupTask.h"

#include "config/user_config.h"
#include "system/SystemManager.h"
#include "system/Watchdog.h"
#include "logging/Loggers.h"

void StartupTask::pollTimeSync()
{
    if (timeSynced) return;

    const auto result = modem.waitFor(ModemResult::UnixTimestamp, 0);
    if (!result.has_value()) return;

    const time_t unixTimestamp = std::get<time_t>(*result);
    const uint32_t uptimeMs = millis();

    Time::syncSystemTime(unixTimestamp);
    timeSynced = true;

    // Matches the calibration message the backend's log parser looks for
    // (app/helpers/log_parser.py: TimeCalibrationMessage / V1_0_0). Every line
    // logged before this point carries a millis()-since-boot timestamp, since
    // there was no wall clock yet; the backend retroactively corrects those
    // using this line's millis-to-unix-time mapping, but only if the format
    // matches exactly. "v1.0.0" is the calibration message format version, not
    // the firmware version — it is the only one the backend currently parses.
    const auto systemTimeMs = static_cast<uint64_t>(unixTimestamp) * 1000ULL;
    logger.infoln("Time (v1.0.0): millis: " + String(uptimeMs) + " ms, Localtime: " +
        Time::toIsoString(systemTimeMs) + " UTC, Unix timestamp: " + String(static_cast<long long>(unixTimestamp)) +
        ", system time: " + String(static_cast<unsigned long long>(systemTimeMs)) + " ms");
}

std::optional<uint> StartupTask::displayApiProgress(const ModemCommand desiredState, const uint32_t hexColor,
                                                    const TickType_t timeoutToReachDesiredState,
                                                    const TickType_t timeToCompleteDesiredState)
{
    TickType_t start = xTaskGetTickCount();

    auto timedOut = [start, timeoutToReachDesiredState]
    {
        return timeoutToReachDesiredState < xTaskGetTickCount() - start;
    };

    while (modem.getCurrentState() != desiredState)
    {
        pollTimeSync();
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
        pollTimeSync();

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
            pollTimeSync();
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
        case ModemCommand::DownloadGPSRfids:
            lastLedCommandId = displayApiProgress(newState, 0xFFA500);
            break;
        case ModemCommand::UploadLog:
            lastLedCommandId = displayApiProgress(newState, 0x0000FF);
            break;
        default:
            break;
        }

        // The queued boot batch is sent once, with no deadline, so nothing in
        // it can be dropped ahead of time — the first time the modem goes idle
        // genuinely means every boot command has at least been attempted, not
        // just that this particular poll happened to land between two of them.
        if (newState == ModemCommand::Ready && !watchdogTightened)
        {
            Watchdog::enterSteadyState();
            watchdogTightened = true;
        }

        pollTimeSync();
        lastState = newState;
    }

    if (lastLedCommandId.has_value())
        led.markCommandAsCompleted(lastLedCommandId.value());

    // Covers the pathological case where every poll point above is skipped
    // (e.g. the outer loop exits on !isRunning() before ever ticking). Costs
    // nothing when already synced.
    pollTimeSync();
}
