#include <Modem.h>
#include <NFCCardReader.h>
#include <HelperUtils.h>
#include <LED.h>
#include <esp32-hal.h>
#include "esp_log.h"
#include "AccessControl.h"
#include "Api.h"
#include "FirmwareUpdater.h"
#include "Globals.h"
#include "GPS.h"
#include "Config.h"
#include "LocalConfig.h"
#include "RFIDs.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"
#include "esp32/rom/rtc.h"

#define DAY_MILLIS 86400000U // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds

ulong nextWatchdogResetMs;
ulong nextGPSUpdate;
ulong restartTargetMs;
uint contiguousFailedSleepAttempts = 0;
uint contiguousFailedDisableGPSAttempts = 0;

ulong lastLogin, lastLogout; // These are volatile

void checkNFCTag()
{
    const auto [status, rfidUid] = cardReader.scan();

    switch (status)
    {
    case ScanStatus::NoCard:
        return;
    case ScanStatus::NewCard:
    case ScanStatus::Duplicate:
        break;
    }

    const ulong firstScanMs = millis();

    if (RFIDs::isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");
        if (accessControl.toggleLogin(rfidUid))
        {
            lastLogin = millis();
            statusLed.unlockFlash();
            if (accessControl.hasPermissionForGPSTracking())
            {
                modem.wakeupAndWait();
                modem.enableGPS();
            }
        }
        else
        {
            lastLogout = millis();
            statusLed.lockFlash();
        }
    }
    else
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        statusLed.cardDeclinedFlash();
    }

    // Wait for 2 seconds for the card to be removed
    constexpr uint waitForRemovalMs = 2000;
    while (millis() - firstScanMs < waitForRemovalMs)
        delay(10);

    // Then check again for two 1 second if a card is present
    constexpr uint waitForScanMs = 1000;
    bool scannedDuplicate = false;
    while (millis() - firstScanMs < waitForRemovalMs + waitForScanMs)
    {
        if (cardReader.scan().status == ScanStatus::Duplicate)
        {
            scannedDuplicate = true;
            break;
        }
    }

    if (!scannedDuplicate) return;

    // If it scanned the same card twice wait another 3 seconds for it to be removed
    // and indicate a cooldown via the LED. The LED starts full brightness cyan and fades out

    constexpr uint showLedMs = 3000;
    const ulong s = millis();

    while (millis() - firstScanMs < waitForRemovalMs + waitForScanMs + showLedMs)
    {
        const ulong t = millis() - s;
        const uint8_t gb = t >= showLedMs ? 0 : 255 - t * 255 / showLedMs;
        statusLed.setColor((gb << 8) | gb);
    }

    // From the first card scan to here it should be 6 seconds

    statusLed.clear();
}

void calculateNextRestartTime()
{
    int hour, minute, second;

    modem.getNetworkTime(nullptr, nullptr, nullptr, &hour, &minute, &second, nullptr);

    // Calculate milliseconds since midnight
    const ulong timeOfDayInMs = (hour * 3600 + minute * 60 + second) * 1000;

    if (timeOfDayInMs < TARGET_TIME_FOR_ESP_RESTART)
    {
        restartTargetMs = TARGET_TIME_FOR_ESP_RESTART - timeOfDayInMs;
    }
    else
    {
        restartTargetMs = DAY_MILLIS - (timeOfDayInMs - TARGET_TIME_FOR_ESP_RESTART);
    }

    restartTargetMs += millis();

    fileLog.infoln("Next restart planned in " + String(restartTargetMs / 1000) + " seconds");
}

int espLogHandler(const char* fmt, const va_list args)
{
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    fileLog.errorln(buf);
    // TODO: Not logging to file, because it could cause an endless loop, when the error message occures from e.g. SD-Card
    return 0;
}

void loadConfig()
{
    const auto loadedConfig = LocalConfig::fromStorage(CONFIG_PREFS_NAME);
    LocalConfig* newConfig;

    if (loadedConfig.has_value())
    {
        newConfig = new LocalConfig{loadedConfig.value()};
    }
    else
    {
        serialOnlyLog.warningln("No or outdated config found. Requesting new config.");
        newConfig = new LocalConfig{HelperUtils::requestConfig()};
        const bool configSaveSuccess = StorableConfig{*newConfig, CONFIG_PREFS_NAME}.save();
        fileLog.logInfoOrErrorln(configSaveSuccess, "Successfully saved config", "Failed to save config");
        WatchdogHandler::taskWDTReset(); // Reset in case the user took long to enter data
    }

    delete config;
    config = newConfig;
}

void checkGPS()
{
    if (!accessControl.hasPermissionForGPSTracking()) return;

    if (LittleFS.totalBytes() - LittleFS.usedBytes() < 128 * 1024)
    {
        // GPS is logging to flash and storage is low
        serialOnlyLog.warningln("Low on flash storage. Not logging GPS");
        nextGPSUpdate = millis() + 5 * 60 * 1000;
        return;
    }

    GPS_DATA_t gpsData;
    modem.getGPS(gpsData);
    gps.writeData(gpsData);
}

void restartRoutine()
{
    fileLog.infoln("Time reached to upload log and restart ESP32");

    statusLed.setStatusColor(StatusColor::UploadingLogs);

    modem.wakeupAndWait();
    modem.ensureNetworkConnection(2, false);

    if (StorageManager::exists(GPS_FILE_PATH))
    {
        gps.uploadFileAndBeginNew(true, true, 2);
    }
    else
    {
        fileLog.infoln("No GPS data recorded. Nothing to upload");
    }

    HelperUtils::uploadLogAndDeleteAfterRetryingIfStorageIsFull();

    fileLog.infoln("Restarting now");

    // NO MORE FILE LOGGING FROM HERE

    swLog.end();

    ESP.restart();
}

void setup()
{
    // Start serial communication
    Serial.begin(USB_SERIAL_BAUD);
    while (!Serial) {}

    if constexpr (ENABLE_SERIAL_LOGGING)
    {
        fileLog.addOutputSink(Serial, "", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
        serialOnlyLog.addOutputSink(Serial, "Serial", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
    }

    // Start watchdog
    watchdogHandler.setTimeout(HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT);
    watchdogHandler.subscribeTask();

    // Mount filesystems
    const bool flashInitSuccess = StorageManager::mountLittleFS();
    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");

    swLog.begin(true);
    fileLog.addOutputSink(swLog, "", true, false, FLASH_LOGGING_LEVEL, true, true);

    // Logging to files is now possible
    esp_log_set_vprintf(&espLogHandler); // Redirect ESP logs to file
    fileLog.infoln("Running firmware version " FIRMWARE_VERSION);
    fileLog.infoln("CPU0 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(0)));
    fileLog.infoln("CPU1 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(1)));

    // Cleanup
    StorageManager::removeGpsLog();

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status
    statusLed.begin();

    // Now let's start the modem and set the system time fetched by the Modem network
    statusLed.setStatusColor(StatusColor::InitializationPhase);
    nfcSpi.begin(NFC_SCLK, NFC_MISO, NFC_MOSI, NFC_SS);
    accessControl.begin();
    gps.begin();
    Serial1.setRxBufferSize(2048);
    Serial1.begin(MODEM_SERIAL_BAUD, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    if constexpr (USE_DEFAULT_CONFIG)
        fileLog.infoln("Using default config");
    else
        loadConfig(); // We need the config for the Modem

    fileLog.infoln("Loaded config: " + config->toString());


    modem.begin(config->simPin.c_str(), config->gprsUser.c_str(), config->gprsPassword.c_str(),
                config->apn.c_str());
    modem.ensureNetworkConnection();
    HelperUtils::syncTimeWithModem(20);


    if (RECORD_GPS_WHILE_STANDING || (accessControl.isLoggedIn() && accessControl.hasPermissionForGPSTracking()))
        modem.enableGPS();

    fileLog.infoln("Signal Quality: " + String(modem.getSignalQuality()));

    fileLog.infoln(
        "Time (v1.0.0): millis: " + String(millis()) + " ms, Localtime: " + modem.getGSMDateTime() +
        ", Unix timestamp: " + String(modem.getUnixTimestamp()) + ", system time: " + String(
            HelperUtils::systemTimeMillisecondsSinceEpoche()) + " ms");
    calculateNextRestartTime();

    // We need the modem IMEI for communicating with the server therefore it is needed before we do anything with the modem
    modemIMEI = modem.getIMEI();
    fileLog.infoln("Modem IMEI: " + modemIMEI);

    api.begin(config->server, config->serverPort, modemIMEI, config->serverPassword);

    // Do the connection speed test before any up-/downloads
    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        HelperUtils::performConnectionSpeedTest(CONNECTION_SPEED_TEST_FILE_SIZE);

    // Now we are ready to check for a firmware update
    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
    {
        statusLed.setStatusColor(StatusColor::PerformingOTAUpdate);
        FirmwareUpdater::doUpdateIfAvailable();
    }
    else
        fileLog.infoln("Skipped firmware update check");

    cardReader.begin();

    // If there is no update we will continue with getting everything ready for reading NFC tags
    statusLed.setStatusColor(StatusColor::UpdatingRFIDs);
    RFIDs::downloadRfidsIfChanged();
    RFIDs::downloadGPSTrackingConsentedRFIDs();
    RFIDs::load();

    HelperUtils::logRAMUsage(fileLog, LoggingLevel::INFO);
    StorageManager::logFilesystemsInformation();

    // Almost everything is done and the created log can be uploaded
    statusLed.setStatusColor(StatusColor::UploadingLogs);
    HelperUtils::uploadLogAndDeleteAfterRetryingIfStorageIsFull();
    statusLed.clear();

    // Power saving
    modem.disconnectNetwork();
    modem.requestSleep();

    // Set the watchdog to a shorter timeout for the main loop
    watchdogHandler.resetTimeout();
    fileLog.infoln("Setup done");
}

void loop()
{
    if (millis() >= nextWatchdogResetMs)
    {
        WatchdogHandler::taskWDTReset();
        nextWatchdogResetMs = millis() + HW_WATCHDOG_RESET_DELAY_MS;
    }

    if (millis() >= restartTargetMs)
        restartRoutine();

    checkNFCTag();

    if (millis() >= nextGPSUpdate)
    {
        if (accessControl.isLoggedIn())
        {
            nextGPSUpdate = millis() + GPS_UPDATE_INTERVAL_WHILE_DRIVING;
            checkGPS();
        }
        else
        {
            nextGPSUpdate = millis() + GPS_UPDATE_INTERVAL_WHILE_STANDING;
            if constexpr (RECORD_GPS_WHILE_STANDING)
                checkGPS();
        }
    }

    if (millis() - lastLogin > 20000 && millis() - lastLogout > 20000)
    {
        if constexpr (!RECORD_GPS_WHILE_STANDING)
        {
            if (contiguousFailedDisableGPSAttempts < 2 && modem.isGPSEnabled() && !(accessControl.isLoggedIn() &&
                accessControl.hasPermissionForGPSTracking()))
            {
                if (modem.disableGPS())
                    contiguousFailedDisableGPSAttempts = 0;
                else
                    ++contiguousFailedDisableGPSAttempts;
            }
        }

        if (contiguousFailedSleepAttempts < 10)
        {
            switch (modem.requestSleep())
            {
            case SleepRequestResult::FailedBecauseModemIsStillInUse:
            case SleepRequestResult::AlreadySleeping:
                break;
            case SleepRequestResult::Failed:
                ++contiguousFailedSleepAttempts;
                break;
            case SleepRequestResult::Success:
                contiguousFailedSleepAttempts = 0;
                break;
            }
        }
    }
}
