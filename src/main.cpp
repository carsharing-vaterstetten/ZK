#include <Arduino.h>
#include <Modem.h>
#include <NFCCardReader.h>
#include <HelperUtils.h>
#include <LED.h>
#include <esp32-hal.h>
#include <esp_system.h>
#include <SD.h>
#include <SPIFFS.h>
#include "esp_log.h"
#include "AccessControl.h"
#include "Config.h"
#include "FirmwareUpdater.h"
#include "Globals.h"
#include "GPS.h"
#include "RFIDs.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"

#define DAY_MILLIS 86400000U // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds

unsigned long nextWatchdogResetMs;
unsigned long nextGPSUpdate;
unsigned long targetMillis;


void checkNFCTag()
{
    uint32_t rfidUid;
    const bool readSuccess = NFCCardReader::readTag(rfidUid);

    if (!readSuccess) return; // No card present

    if (RFIDs::isRegisteredRFID(rfidUid))
    {
        fileLog.infoln("Scanned known RFID card: '" + String(rfidUid, 16) + "'");
        if (isLoggedIn)
        {
            AccessControl::logout();
        }
        else
        {
            AccessControl::login(rfidUid);
            currentRFIDConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(rfidUid);
            fileLog.infoln(currentRFIDConsentsToGPSTracking
                               ? "Logged in RFID consents to GPS tracking"
                               : "Logged in RFID does not consent to GPS tracking");
        }
    }
    else
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        statusLed.setColor(Color::Red);
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

void enableFileLogging(const bool forceFlash)
{
    if (forceFlash)
    {
        StorageManager::setFS(SPIFFS, SPIFFS, SPIFFS, SPIFFS, SPIFFS);

        fileLog.enableFlashLogging(LOG_FILE_PATH, FLASH_LOGGING_LEVEL);

        fileLog.infoln("Forced to use flash. Now logging to file(s)");
        return;
    }

    StorageManager::setFS(SD, SPIFFS, SD, SD, SPIFFS);

    fileLog.enableSDCardLogging(LOG_FILE_PATH, SD_CARD_LOGGING_LEVEL);

    fileLog.infoln("Now logging to file(s)");
}

void initializeStorage()
{
    if (config.preferSDCard)
    {
        if (StorageManager::isSDCardConnected())
        {
            serialOnlyLog.infoln("Using SD-Card as preferred storage");

            // Should already be mounted after isSDCardConnected()
            const bool sdInitSuccess = StorageManager::mountSDCard();

            if (!sdInitSuccess)
            {
                serialOnlyLog.warningln("Failed to initialize SD-Card");
            }

            enableFileLogging(!sdInitSuccess);
        }
        else
        {
            serialOnlyLog.warningln("SD-Card is not inserted");
            enableFileLogging(true);
        }
    }
    else
    {
        serialOnlyLog.infoln("Using flash as preferred storage");
        enableFileLogging(true);
    }
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
#if OVERRIDE_CONFIG
    serialOnlyLog.infoln("Using compiled config");
#else
    const bool configLoadedSuccess = StorageManager::loadConfigFromEEPROM(config);

    if (!configLoadedSuccess)
    {
        serialOnlyLog.warningln("No or outdated config found. Requesting new config.");
        HelperUtils::requestConfig(config);
        WatchdogHandler::taskWDTReset(); // Reset in case the user took long to enter data
        StorageManager::saveConfigToEEPROM(config);
    }
#endif
}

void checkGPS()
{
    if (isLoggedIn && !currentRFIDConsentsToGPSTracking) return;

    if (StorageManager::gpsFs == &SPIFFS && SPIFFS.totalBytes() - SPIFFS.usedBytes() < 128 * 1024)
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
        serialOnlyLog.debugln("No GPS data received");
        return;
    }

    serialOnlyLog.debugln("Lat: " + String(gpsData.lat, 11) + " Long: " + String(gpsData.lon, 11));

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
    const bool flashInitSuccess = StorageManager::mountSSPIFFS();
    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");
    const bool eepromInitSuccess = StorageManager::mountEEPROM();
    serialOnlyLog.logInfoOrCriticalErrorln(eepromInitSuccess, "EEPROM initialized successfully",
                                           "EEPROM initialization failed");
    loadConfig(); // Config is now needed because it contains information whether the SD-Card should be used
    serialOnlyLog.infoln("Loaded config: " + HelperUtils::getConfigHumanReadable(config));
    initializeStorage();

    // Logging to files is now possible
    esp_log_set_vprintf(&espLogHandler); // Redirect ESP logs to file
    fileLog.infoln("Loaded config: " + HelperUtils::getConfigHumanReadableHideSecrets(config));
    fileLog.infoln("Running firmware version " FIRMWARE_VERSION);
    fileLog.infoln("Hardware startup reason: " + WatchdogHandler::getResetReasonHumanReadable(esp_reset_reason()));
    StorageManager::logFSConfiguration();
    StorageManager::logFilesystemsInformation();

    // Cleanup
    StorageManager::removeFirmwareFile();
    StorageManager::removeGpsLog();

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status
    statusLed.init();

    // We need the efuseMac for communicating with the server therefore it is needed before we do anything with the modem
    efuseMac = ESP.getEfuseMac();
    efuseMacHex = String(efuseMac, 16);
    fileLog.infoln("Efuse chip ID: 0x" + efuseMacHex);

    // Now let's start the modem and set the system time fetched by the Modem network
    statusLed.setStatusColor(StatusColor::InitializationPhase);
    Modem::init();
    HelperUtils::updateSystemTimeWithModem();
    fileLog.infoln(
        "Time: millis: " + String(millis()) + " ms, Localtime: " + Modem::getGSMDateTime() +
        ", Unix timestamp: " + String(Modem::getUnixTimestamp()) + ", system time: " + String(
            HelperUtils::systemTimeMillisecondsSinceEpoche()) + " ms");
    calculateNextRestartTime();

    // Do the connection speed test before any up-/downloads
#if !SKIP_INITIAL_CONNECTION_SPEED_TEST
    Modem::performConnectionSpeedTest();
#endif

    // Now we are ready to check for a firmware update
    statusLed.setStatusColor(StatusColor::PerformingOTAUpdate);
    FirmwareUpdater::doUpdateIfAvailable();

    // If there is no update we will continue with getting everything ready for reading NFC tags
    statusLed.setStatusColor(StatusColor::UpdatingRFIDs);
    AccessControl::init();
    NFCCardReader::init();
    RFIDs::downloadRfidsIfChanged();
    RFIDs::downloadGPSTrackingConsentedRFIDs();

    // Almost everything is done and the created log can be uploaded
    statusLed.setStatusColor(StatusColor::UploadingLogs);
    Modem::uploadLogsFromAllFileSystems(false, true, 1);
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
        Modem::uploadLogsFromAllFileSystems(true, false, 10); // Log will be deleted at next startup anyway

        ESP.restart();
    }

    checkNFCTag();

    if (millis() >= nextGPSUpdate)
    {
        nextGPSUpdate = millis() + (
            isLoggedIn ? GPS_UPDATE_INTERVAL_WHILE_DRIVING : GPS_UPDATE_INTERVAL_WHILE_STANDING);
        checkGPS();
    }
}
