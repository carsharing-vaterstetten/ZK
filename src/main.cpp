#include <Arduino.h>
#include <Modem.h>
#include <NFCCardReader.h>
#include <HelperUtils.h>
#include <LED.h>
#include <esp32-hal.h>
#include "esp_log.h"
#include "AccessControl.h"
#include "Config.h"
#include "FirmwareUpdater.h"
#include "Globals.h"
#include "GPS.h"
#include "RFIDs.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"
#include "esp32/rom/rtc.h"

#define DAY_MILLIS 86400000U // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds

unsigned long nextWatchdogResetMs;
unsigned long nextGPSUpdate;
unsigned long targetMillis;


void checkNFCTag()
{
    uint32_t rfidUid;

    if (!NFCCardReader::readTag(rfidUid)) return; // No card present

    if (RFIDs::isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");
        accessControl.toggleLogin(rfidUid);
    }
    else
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        statusLed.setStatusColor(StatusColor::NFCUnknownUIDScanned);
    }

    delay(2000);

    statusLed.clear();
}

void calculateNextRestartTime()
{
    int hour, minute, second;

    Modem::getNetworkTime(nullptr, nullptr, nullptr, &hour, &minute, &second, nullptr);

    // Calculate milliseconds since midnight
    const unsigned long currentMillis = (hour * 3600 + minute * 60 + second) * 1000;

    if (currentMillis < TARGET_TIME_FOR_ESP_RESTART)
    {
        targetMillis = TARGET_TIME_FOR_ESP_RESTART - currentMillis;
    }
    else
    {
        targetMillis = DAY_MILLIS - (currentMillis - TARGET_TIME_FOR_ESP_RESTART);
    }

    targetMillis += millis();

    fileLog.infoln("Next restart planed in " + String(targetMillis / 1000) + " seconds");
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
#if USE_DEFAULT_CONFIG
    fileLog.infoln("Using default config");
#else

    if (const auto loadedConfig = LocalConfig::fromStorage(); !loadedConfig)
    {
        serialOnlyLog.warningln("No or outdated config found. Requesting new config.");
        config = HelperUtils::requestConfig();
        const bool configSaveSuccess = config.save();
        fileLog.logInfoOrErrorln(configSaveSuccess, "Successfully saved config", "Failed to save config");
        WatchdogHandler::taskWDTReset(); // Reset in case the user took long to enter data
    }
#endif
    fileLog.infoln("Loaded config: " + config.toString());
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
    const bool gpsSuccess = Modem::getGPS(gpsData);

    if (!gpsSuccess)
    {
        // serialOnlyLog.debugln("No GPS data received");
        return;
    }

    // serialOnlyLog.debugln("Lat: " + String(gpsData.lat, 11) + " Long: " + String(gpsData.lon, 11));

    GPS::logDataBuffered(gpsData);
}

void setup()
{
    // Start serial communication
    Serial.begin(UART_BAUD);
    while (!Serial)
    {
    }

#if ENABLE_SERIAL_LOGGING
    fileLog.enableSerialLogging(SERIAL_LOGGING_LEVEL);
    serialOnlyLog.enableSerialLogging(SERIAL_LOGGING_LEVEL, "Serial");
#endif

    // Start watchdog
    WatchdogHandler::setTimeout(HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT);
    WatchdogHandler::subscribeTask();

    // Mount filesystems
    const bool flashInitSuccess = StorageManager::mountLittleFS();
    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");

    const bool enableFileLoggingSuccess = fileLog.enableFlashLogging(LOG_FILE_PATH, FLASH_LOGGING_LEVEL);
    fileLog.logInfoOrErrorln(enableFileLoggingSuccess, "Now logging to file(s)", "Failed to enable file logging");

    // Logging to files is now possible
    esp_log_set_vprintf(&espLogHandler); // Redirect ESP logs to file
    fileLog.infoln("Running firmware version " FIRMWARE_VERSION);
    fileLog.infoln("CPU0 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(0)));
    fileLog.infoln("CPU1 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(1)));

    StorageManager::logFilesystemsInformation();

    // Cleanup
    StorageManager::removeGpsLog();

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status
    statusLed.init();

    // Now let's start the modem and set the system time fetched by the Modem network
    statusLed.setStatusColor(StatusColor::InitializationPhase);
    loadConfig(); // We need the config for the Modem
    Modem::init();
    fileLog.infoln(
        "Time (v1.0.0): millis: " + String(millis()) + " ms, Localtime: " + Modem::getGSMDateTime() +
        ", Unix timestamp: " + String(Modem::getUnixTimestamp()) + ", system time: " + String(
            HelperUtils::systemTimeMillisecondsSinceEpoche()) + " ms");
    calculateNextRestartTime();

    // We need the modem IMEI for communicating with the server therefore it is needed before we do anything with the modem
    modemIMEI = Modem::getIMEI();
    fileLog.infoln("Modem IMEI: " + modemIMEI);

    // Do the connection speed test before any up-/downloads
#if !SKIP_INITIAL_CONNECTION_SPEED_TEST
    Modem::performConnectionSpeedTest();
#endif

    // Now we are ready to check for a firmware update
#if CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT
    statusLed.setStatusColor(StatusColor::PerformingOTAUpdate);
    FirmwareUpdater::doUpdateIfAvailable();
#else
    fileLog.infoln("Skipped firmware update check");
#endif

    // If there is no update we will continue with getting everything ready for reading NFC tags
    statusLed.setStatusColor(StatusColor::UpdatingRFIDs);
    accessControl.init();
    NFCCardReader::init();
    RFIDs::downloadRfidsIfChanged();
    RFIDs::downloadGPSTrackingConsentedRFIDs();

    // Almost everything is done and the created log can be uploaded
    statusLed.setStatusColor(StatusColor::UploadingLogs);
    Modem::uploadLog(true, true, 1);
    statusLed.clear();

    // Set the watchdog to a shorter timeout for the main loop
    WatchdogHandler::resetTimeout();
    fileLog.infoln("Setup done");
}

void loop()
{
    if (millis() >= nextWatchdogResetMs)
    {
        WatchdogHandler::taskWDTReset();
        nextWatchdogResetMs = millis() + HW_WATCHDOG_RESET_DELAY_MS;
    }

    if (millis() >= targetMillis)
    {
        fileLog.infoln("Time reached to upload log and restart ESP32");

        statusLed.setStatusColor(StatusColor::UploadingLogs);
        Modem::performConnectionSpeedTest();
        GPS::uploadFileAndDelete(true, true, 2);
        Modem::uploadLog(true, false, 10); // Log will be deleted at next startup anyway

        ESP.restart();
    }

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
#if RECORD_GPS_WHILE_STANDING
            checkGPS();
#endif
        }
    }
}
