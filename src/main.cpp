#include <Arduino.h>
#include <Modem.h>
#include <NFCCardReader.h>
#include <HelperUtils.h>
#include <LED.h>
#include <esp32-hal.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <SD.h>
#include <SPIFFS.h>
#include "esp_log.h"
#include "AccessControl.h"
#include "FirmwareUpdater.h"
#include "Globals.h"
#include "RFIDs.h"
#include "StorageManager.h"

#define DAY_MILLIS 86400000U // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds

unsigned long nextWatchdogResetMs;
unsigned long nextGPSUpdate;
unsigned long targetMillis;


void checkNFCTag()
{
    const uint32_t readValue = NFCCardReader::readTag();

    if (readValue == 0) return; // No card present

    if (RFIDs::isRegisteredRFID(readValue))
    {
        fileLog.infoln("Scanned known RFID card: '" + String(readValue, 16) + "'");
        if (isLoggedIn)
        {
            AccessControl::logout();
        }
        else
        {
            AccessControl::login(readValue);
            currentRFIDConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(readValue);
            fileLog.infoln(currentRFIDConsentsToGPSTracking
                               ? "Logged in RFID consents to GPS tracking"
                               : "Logged in RFID does not consent to GPS tracking");
        }
    }
    else
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(readValue, 16) + "'");
        statusLed.setColor(Color::Red);
    }

    delay(2000);

    statusLed.clear();
}

void initTime()
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
        gpsLog.enableFlashLogging(GPS_FILE_PATH);

        fileLog.infoln("Forced to use flash. Now logging to file(s)");
        return;
    }

    StorageManager::setFS(SD, SPIFFS, SD, SD, SPIFFS);

    fileLog.enableSDCardLogging(LOG_FILE_PATH, SD_CARD_LOGGING_LEVEL);
    gpsLog.enableSDCardLogging(GPS_FILE_PATH);

    fileLog.infoln("Now logging to file(s)");
}

int espLogHandler(const char* fmt, const va_list args)
{
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    fileLog.errorln(buf);
    // TODO: Not logging to file, because it could cause an endless loop, when the error message occures from e.g. SD-card
    return 0;
}

void setup()
{
    Serial.begin(UART_BAUD);
    while (!Serial)
    {
    }

#if ENABLE_SERIAL_LOGGING
    fileLog.enableSerialLogging(SERIAL_LOGGING_LEVEL);
    serialOnlyLog.enableSerialLogging(SERIAL_LOGGING_LEVEL);
#endif

    const esp_reset_reason_t reset_reason = esp_reset_reason();

    // Initialize the watchdog.
    // WARNING: If this setup function does not complete within the given HW_WATCHDOG_TIMEOUT the watchdog will perform a reset.
    // That could possibly lead to an infinite resetting loop.
    HelperUtils::setWatchdog(HW_WATCHDOG_DEFAULT_TIMEOUT);
    // Add the current task to be monitored by the watchdog.
    // This ensures that if the main loop doesn't reset the watchdog in time,
    // the ESP32 will reset itself.
    HelperUtils::subscribeTaskToWatchdog();

    // redirect ESP logs
    esp_log_set_vprintf(&espLogHandler);

    const bool flashInitSuccess = StorageManager::mountSSPIFFS();
    StorageManager::mountEEPROM();

    const bool configLoadedSuccess = StorageManager::loadConfigFromEEPROM(config);

    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialization succeeded",
                                           "Flash initialization failed");

    if (!configLoadedSuccess)
    {
        serialOnlyLog.warningln("No or outdated config found. Requesting new config.");
        HelperUtils::requestConfig(config);
        StorageManager::saveConfigToEEPROM(config);
    }

    if (StorageManager::isSDCardConnected() && config.preferSDCard)
    {
        serialOnlyLog.infoln("Using SD-card as preferred storage");

        const bool sdInitSuccess = StorageManager::mountSDCard(); // Should already be mounted after isSDCardConnected()
        if (!sdInitSuccess)
        {
            serialOnlyLog.warningln("Failed to initialize SD-card");
        }
        enableFileLogging(!sdInitSuccess);
    }
    else
    {
        serialOnlyLog.infoln("Using flash as preferred storage");
        enableFileLogging(true);
    }

    fileLog.infoln("Loaded config: " + HelperUtils::getConfigHumanReadable(config));

    fileLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialization succeeded",
                                     "Flash initialization failed");

    fileLog.infoln("Running firmware version " FIRMWARE_VERSION);

    fileLog.infoln("Hardware startup reason: " + HelperUtils::getResetReasonHumanReadable(reset_reason));

    statusLed.init();

    statusLed.setColor(Color::White);

    efuseMac = ESP.getEfuseMac();
    efuseMacHex = String(efuseMac, 16);

    fileLog.infoln("Efuse chip ID: 0x" + efuseMacHex);

    Modem::init();

    fileLog.infoln(
        "Time: millis: " + String(millis()) + " ms, Localtime: " + Modem::getLocalTime() +
        ", UTC: " + String(Modem::getUTCTimestamp()));

    StorageManager::removeFirmwareFile(); // Cleanup

    statusLed.setColor(Color::Purple);

    FirmwareUpdater::doUpdateIfAvailable();

    statusLed.setColor(Color::Orange);

    AccessControl::init();

    NFCCardReader::init();

    initTime();

    fileLog.infoln("Initialization phase complete.");

    statusLed.setColor(Color::Blue);

    RFIDs::downloadRfidsIfChanged();
    RFIDs::downloadGPSTrackingConsentedRFIDs();

    Modem::uploadLog(true, true, 1);

    statusLed.clear();

    fileLog.infoln("Setup done");
}

void loop()
{
    if (millis() >= nextWatchdogResetMs)
    {
        esp_task_wdt_reset(); // Reset the watchdog timer
        nextWatchdogResetMs = millis() + HW_WATCHDOG_RESET_DELAY_MS;
    }

    if (millis() >= targetMillis)
    {
        fileLog.infoln("Time reached to upload log and restart ESP32");

        statusLed.setStatusColor(StatusColor::PerformingOTAUpdate);

        Modem::uploadGPSFile(true, true, 3);
        Modem::uploadLog(true, false, 10); // Log will be deleted at next startup anyways

        ESP.restart();
    }

    checkNFCTag();

    if (millis() >= nextGPSUpdate)
    {
        if (!isLoggedIn || currentRFIDConsentsToGPSTracking)
        {
            GPS_DATA_t gpsData;
            const bool gpsSuccess = Modem::getGPS(gpsData);

            if (gpsSuccess)
            {
                const auto parsedBytes = reinterpret_cast<uint8_t*>(&gpsData);
                serialOnlyLog.debugln("Lat: " + String(gpsData.lat, 11) + " Long: " + String(gpsData.lon, 11));
                gpsLog.write(parsedBytes, sizeof(GPS_DATA_t));
            }
            else
            {
                serialOnlyLog.debugln("Failed to get GPS");
            }
        }

        nextGPSUpdate = millis() + (
            isLoggedIn ? GPS_UPDATE_INTERVAL_WHILE_DRIVING : GPS_UPDATE_INTERVAL_WHILE_STANDING);
    }
}
