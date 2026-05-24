#include <esp32-hal.h>
#include <esp_task_wdt.h>

#include "modules/Modem.h"
#include "modules/NFCCardReader.h"
#include "modules/LED.h"
#include "HelperUtils.h"
#include "esp_log.h"
#include "modules/AccessControl.h"
#include "modules/Api.h"
#include "config/Backend.h"
#include "FirmwareUpdater.h"
#include "Globals.h"
#include "GPS.h"
#include "GPSAlg.h"
#include "LocalConfig.h"
#include "RFIDs.h"
#include "modules/SwappableFile.h"
#include "LittleFSHelper.h"
#include "config/hw_config.h"
#include "config/Intern.h"
#include "esp32/rom/rtc.h"
#include "config/Config.h"
#include "config/user_config.h"

#define DAY_MILLIS 86400000U // [ms] = 24 * 60 * 60 * 1000 -> a day in milliseconds

enum class TaskStatus
{
    NotStarted,
    Running,
    StopRequested,
    Stopped,
};

ulong restartTargetMs;
uint contiguousFailedSleepAttempts = 0;
uint contiguousFailedDisableGPSAttempts = 0;

ulong lastLogin, lastLogout; // These are volatile

GPSAlgPrediction lastGpsState;

volatile TaskStatus nfcScanningTaskStatus = TaskStatus::NotStarted, gpsDataTaskStatus = TaskStatus::NotStarted;

#ifdef HW_REV
constexpr const BoardConfig* ACTIVE_BOARD = activeBoard<HW_REV>();
#else
const BoardConfig* ACTIVE_BOARD;
#endif

TinyGsmSim7000 gsmModem{Serial1};
std::optional<ModemDriver> modemDriver;
std::optional<Modem> modem;
std::optional<CarKeyDriver> carKeyDriver;
std::optional<AccessControl> accessControl;
GPS gps{GPS_FILE_PATH, GPS_FILE_UPLOAD_ENDPOINT};
std::optional<ApiClient> api;
std::optional<Adafruit_NeoPixel> ledDriver;
std::optional<CardReaderLED> statusLed;

std::optional<LocalConfig> config;
SwappableFile swLog{PRIMARY_LOG_FILE_PATH, SECONDARY_LOG_FILE_PATH};
GPSAlg gpsAlg{};


void checkNFCTag(NFCCardReader& cardReader, bool detected)
{
    const auto [status, rfidUid] = cardReader.scan(detected);

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
        if (accessControl->toggleLogin(rfidUid))
        {
            lastLogin = millis();
            statusLed->unlockFlash();

            if (accessControl->hasPermissionForGPSTracking())
            {
                modem->wakeupAndWait();
                modem->enableGPS();

                if (!gpsAlg.isTripActive())
                {
                    gpsAlg.startTrip();
                    fileLog.infoln("Trip started");
                }
            }
        }
        else
        {
            lastLogout = millis();
            statusLed->lockFlash();

            if (gpsAlg.isTripActive())
            {
                const float traveledDistance = gpsAlg.endTrip();
                fileLog.infoln("Trip ended. Traveled distance: " + String(traveledDistance) + " m");
            }
        }
    }
    else
    {
        fileLog.infoln("Scanned unknown RFID card: '" + String(rfidUid, 16) + "'");
        statusLed->cardDeclinedFlash();
    }

    // Wait for 2 seconds for the card to be removed
    constexpr uint waitForRemovalMs = 2000;
    delay(waitForRemovalMs);

    // Then check again for two 1 second if a card is present
    constexpr uint waitForScanMs = 1000;
    bool scannedDuplicate = false;
    while (millis() - firstScanMs < waitForRemovalMs + waitForScanMs)
    {
        if (cardReader.scan(false).status == ScanStatus::Duplicate)
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
        float progress = 1.0f - static_cast<float>(millis() - s) / static_cast<float>(showLedMs);
        statusLed->progressIndicatorNext(StatusColor::WaitingForNFCCardToBeRemoved, std::clamp(progress, 0.0f, 1.0f));
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // From the first card scan to here it should be 6 seconds

    statusLed->progressIndicatorStop();
}

[[noreturn]] void restartRoutine()
{
    fileLog.infoln("Time reached to upload log and restart ESP32");

    statusLed->setStatusColor(StatusColor::UploadingLogs);

    // Stop other tasks
    nfcScanningTaskStatus = TaskStatus::StopRequested;
    gpsDataTaskStatus = TaskStatus::StopRequested;

    fileLog.debugln("Waiting for tasks to stop");

    while (nfcScanningTaskStatus == TaskStatus::StopRequested || gpsDataTaskStatus == TaskStatus::Stopped)
        vTaskDelay(pdMS_TO_TICKS(100));

    modem->wakeupAndWait();
    modem->ensureNetworkConnection();

    if (LittleFS.exists(GPS_FILE_PATH))
        gps.uploadFileAndBeginNew(*api, true, true, 2);
    else
        fileLog.infoln("No GPS data recorded. Nothing to upload");

    HelperUtils::uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(*api, swLog);

    fileLog.infoln("Restarting now");

    // NO MORE FILE LOGGING FROM HERE

    swLog.end();

    ESP.restart();

    while (true); // to disable no return warning
}

void calculateNextRestartTime()
{
    int hour, minute, second;

    modem->getNetworkTime(nullptr, nullptr, nullptr, &hour, &minute, &second, nullptr);

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

    if (loadedConfig.has_value())
    {
        config.emplace(loadedConfig.value());
    }
    else
    {
        serialOnlyLog.warningln("No or outdated config found. Requesting new config.");
        esp_task_wdt_delete(nullptr); // remove wdt while waiting for config
        config.emplace(HelperUtils::requestConfig());
        esp_task_wdt_add(nullptr);
        esp_task_wdt_reset();
        const bool configSaveSuccess = StorableConfig{config.value(), CONFIG_PREFS_NAME}.save();
        fileLog.logInfoOrErrorln(configSaveSuccess, "Successfully saved config", "Failed to save config");
    }
}

void checkGPS()
{
    if (!accessControl->hasPermissionForGPSTracking()) return;

    if (LittleFS.totalBytes() - LittleFS.usedBytes() < 128 * 1024)
    {
        // GPS is logging to flash and storage is low
        serialOnlyLog.warningln("Low on flash storage. Not logging GPS");
        return;
    }

    GPS_DATA_t gpsData;
    modem->getGPS(gpsData);
    gps.writeData(gpsData);
    const GPSAlgPrediction gpsState = gpsAlg.pushData(gpsData);
    if (gpsState != lastGpsState)
    {
        fileLog.infoln("Car state changed to " + GPSAlg::gpsAlgPredictionToStr(gpsState));
        lastGpsState = gpsState;
    }
}


void cardScanTask(void*)
{
    esp_task_wdt_add(nullptr);
    nfcScanningTaskStatus = TaskStatus::Running;

    fileLog.debugln("Card scanner task started");

    SPIClass nfcSpi{ACTIVE_BOARD->nfcSpi};
    Adafruit_PN532 nfcDriver{ACTIVE_BOARD->nfcCs, &nfcSpi};
    NFCCardReader cardReader{nfcDriver};

    nfcSpi.begin(ACTIVE_BOARD->nfcClk, ACTIVE_BOARD->nfcMiso, ACTIVE_BOARD->nfcMosi, ACTIVE_BOARD->nfcCs);
    cardReader.begin();

    while (nfcScanningTaskStatus != TaskStatus::StopRequested)
    {
        checkNFCTag(cardReader, false);
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    nfcScanningTaskStatus = TaskStatus::Stopped;
    fileLog.debugln("Card scanner task ended");

    esp_task_wdt_delete(nullptr);
    vTaskDelete(nullptr);
}

[[noreturn]] void restartTask(void*)
{
    fileLog.debugln("Restart task started");
    while (true)
    {
        if (millis() >= restartTargetMs)
        {
            esp_task_wdt_add(nullptr);
            restartRoutine();
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void gpsDataTask(void*)
{
    esp_task_wdt_add(nullptr);

    gpsDataTaskStatus = TaskStatus::Running;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    fileLog.debugln("GPS data task started");

    while (gpsDataTaskStatus != TaskStatus::StopRequested)
    {
        TickType_t xFrequency;

        if (accessControl->isLoggedIn())
        {
            xFrequency = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_DRIVING);
            checkGPS();
        }
        else
        {
            xFrequency = pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_WHILE_STANDING);
            if constexpr (RECORD_GPS_WHILE_STANDING)
                checkGPS();
        }

        esp_task_wdt_reset();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    fileLog.debugln("GPS data task ended");
    gpsDataTaskStatus = TaskStatus::Stopped;

    esp_task_wdt_delete(nullptr);
    vTaskDelete(nullptr);
}

void setup()
{
    // Start serial communication
    Serial.begin(USB_SERIAL_BAUD);
    while (!Serial) {}

#ifndef HW_REV
    Serial.println("No hardware revision specified. Please enter revision number:");
    Serial.setTimeout(100000);

    while (ACTIVE_BOARD == nullptr)
    {
        if (Serial.available())
        {
            const int rev = Serial.readStringUntil('\n').toInt();
            const BoardConfig* board = getBoard(rev);
            if (board != nullptr)
            {
                ACTIVE_BOARD = board;
                Serial.println("Using board revision " + String(rev));
            }
            else
            {
                Serial.println("Unknown revision, try again:");
            }
        }
    }
#endif

    if constexpr (ENABLE_SERIAL_LOGGING)
    {
        fileLog.addOutputSink(Serial, "", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
        serialOnlyLog.addOutputSink(Serial, "Serial", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
    }

    // Start watchdog
    esp_task_wdt_init(HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT, true);
    esp_task_wdt_add(nullptr);

    // Mount filesystems
    const bool flashInitSuccess = LittleFS.begin(true);
    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");

    swLog.begin(true);
    fileLog.addOutputSink(swLog, "", true, false, FLASH_LOGGING_LEVEL, true, true);

    // Logging to files is now possible
    esp_log_set_vprintf(&espLogHandler); // Redirect ESP logs to file
    fileLog.infoln("Running firmware version " FIRMWARE_VERSION);
    const RESET_REASON cpu0ResetReason = rtc_get_reset_reason(0);
    fileLog.infoln("CPU0 reset reason: " + HelperUtils::getResetReasonHumanReadable(cpu0ResetReason));
    fileLog.infoln("CPU1 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(1)));

    // Cleanup
    LittleFSHelper::remove(GPS_FILE_PATH);

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status
    ledDriver.emplace(ACTIVE_BOARD->ledCount, ACTIVE_BOARD->led, NEO_GRB + NEO_KHZ800);
    statusLed.emplace(*ledDriver);

    statusLed->begin();

    // Now let's start the modem and set the system time fetched by the Modem network
    statusLed->setStatusColor(StatusColor::InitializationPhase);

    carKeyDriver.emplace(*ACTIVE_BOARD);
    accessControl.emplace(*carKeyDriver);

    accessControl->begin();
    gps.begin();
    Serial1.setRxBufferSize(2048);
    Serial1.begin(MODEM_SERIAL_BAUD, SERIAL_8N1, ACTIVE_BOARD->modemRx, ACTIVE_BOARD->modemTx);

    if constexpr (USE_DEFAULT_CONFIG)
    {
        config.emplace(DEFAULT_CONFIG_APN, DEFAULT_CONFIG_GPRS_USER, DEFAULT_CONFIG_GPRS_PASSWORD,
                       DEFAULT_CONFIG_SERVER, DEFAULT_CONFIG_PORT, DEFAULT_CONFIG_PASSWORD, DEFAULT_SIM_PIN);
        fileLog.infoln("Using default config");
    }
    else
        loadConfig(); // We need the config for the Modem

    fileLog.infoln("Loaded config: " + config->toString());


    modemDriver.emplace(*ACTIVE_BOARD);
    modem.emplace(gsmModem, Serial1, MODEM_SERIAL_BAUD, *modemDriver);

    modem->begin(config->simPin.c_str(), config->gprsUser.c_str(), config->gprsPassword.c_str(),
                 config->apn.c_str());
    // In my tests connecting network first, then gprs is best after a hard reset (e.g. code upload). The other order after a ESP.restart().
    modem->ensureNetworkConnection(cpu0ResetReason != POWERON_RESET);
    HelperUtils::syncTimeWithModem(*modem, 20);


    if (RECORD_GPS_WHILE_STANDING || (accessControl->isLoggedIn() && accessControl->hasPermissionForGPSTracking()))
        modem->enableGPS();

    fileLog.infoln("Signal Quality: " + String(modem->getSignalQuality()));

    fileLog.infoln(
        "Time (v1.0.0): millis: " + String(millis()) + " ms, Localtime: " + modem->getGSMDateTime() +
        ", Unix timestamp: " + String(modem->getUnixTimestamp()) + ", system time: " + String(
            HelperUtils::systemTimeMillisecondsSinceEpoche()) + " ms");
    calculateNextRestartTime();

    // We need the modem IMEI for communicating with the server therefore it is needed before we do anything with the modem
    String modemIMEI = modem->getIMEI();
    fileLog.infoln("Modem IMEI: " + modemIMEI);

    auto* gsmClient = new TinyGsmSim7000::GsmClientSim7000{gsmModem};
    auto* modemClient = new WdClient{*gsmClient, config->server, config->serverPort};
    api.emplace(*modemClient, modemIMEI, config->serverPassword);

    // Do the connection speed test before any up-/downloads
    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        HelperUtils::performConnectionSpeedTest(*api,CONNECTION_SPEED_TEST_FILE_SIZE);

    // Now we are ready to check for a firmware update
    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
    {
        statusLed->setStatusColor(StatusColor::PerformingOTAUpdate);
        FirmwareUpdater::doUpdateIfAvailable(*api);
    }
    else
        fileLog.infoln("Skipped firmware update check");

    // If there is no update we will continue with getting everything ready for reading NFC tags
    statusLed->setStatusColor(StatusColor::UpdatingRFIDs);
    RFIDs::downloadRfidsIfChanged(*api);
    RFIDs::downloadGPSTrackingConsentedRFIDs(*api);
    RFIDs::load();

    HelperUtils::logRAMUsage(fileLog, LoggingLevel::INFO);
    LittleFSHelper::logFilesystemsInformation();

    // Almost everything is done and the created log can be uploaded
    statusLed->setStatusColor(StatusColor::UploadingLogs);
    HelperUtils::uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(*api, swLog);
    statusLed->clear();

    // Power saving
    modem->disconnectNetwork();
    modem->requestSleep();

    // Start tasks
    esp_task_wdt_deinit();
    esp_task_wdt_init(HW_WATCHDOG_DEFAULT_TIMEOUT, true); // update timeout
    xTaskCreatePinnedToCore(cardScanTask, "NFC", 4096, nullptr, 3, nullptr, 1);
    xTaskCreatePinnedToCore(gpsDataTask, "GPS", 4096, nullptr, 2, nullptr, 1);
    xTaskCreatePinnedToCore(restartTask, "RST", 4096, nullptr, 1, nullptr, 1);

    // Set the watchdog to a shorter timeout for the main loop
    fileLog.infoln("Setup done");
    esp_task_wdt_delete(nullptr);
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
