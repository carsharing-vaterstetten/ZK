#include <atomic>
#include <esp32-hal.h>
#include <esp_task_wdt.h>

#include "modules/Modem.h"
#include "modules/NFCCardReader.h"
#include "HelperUtils.h"
#include "esp_log.h"
#include "modules/Api.h"
#include "shared/Globals.h"
#include "modules/GPS.h"
#include "shared/GPSAlg.h"
#include "shared/KeySequenceManager.h"
#include "shared/LocalConfig.h"
#include "shared/RFIDs.h"
#include "shared/SwappableFile.h"
#include "LittleFSHelper.h"
#include "SystemManager.h"
#include "config/hw_config.h"
#include "config/Intern.h"
#include "esp32/rom/rtc.h"
#include "config/Config.h"
#include "config/user_config.h"
#include "services/CardReaderService.h"
#include "tasks/AccessControlTask.h"
#include "tasks/GpsTask.h"
#include "tasks/RestartTask.h"

enum class TaskStatus
{
    NotStarted,
    Running,
    StopRequested,
    Stopped,
};

#ifdef HW_REV
constexpr const BoardConfig* ACTIVE_BOARD = activeBoard<HW_REV>();
#else
const BoardConfig* ACTIVE_BOARD;
#endif


// Shared
std::optional<LocalConfig> config;
GPSAlg gpsAlg{};
RFIDs rfidsManager{"/rfids.bin", "/tmp_rfids.bin", "/rfids_gps_consent.bin", "/tmp_rfids_gps_consent.bin"};
KeySequenceManager keySequenceManager;
AccessStatus accessStatus{"AccCtrl v1", rfidsManager};

// Drivers
std::optional<Adafruit_PN532> pn532Driver;
std::optional<CarKeyDriver> carKeyDriver;
std::optional<Adafruit_NeoPixel> ledDriver;

// Modules
std::optional<NFCCardReader> cardReaderModule;
std::optional<KeyControl> keyControlModule;
std::optional<CardReaderLED> ledModule;

// Services
std::optional<CardReaderService> cardReaderService;
std::optional<KeyControlService> keyControlService;
std::optional<LedService> ledService;
std::optional<ModemService> modemService;

// Tasks
std::optional<AccessControlTask> acTask;
std::optional<GPSTask> gpsTask;
std::optional<RestartTask> restartTask;

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

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status

    accessStatus.begin();

    rfidsManager.loadFromFileToRam();
    rfidsManager.loadFromGpsFileToRam();
    keySequenceManager.loadSequenceInRAM(*ACTIVE_BOARD); // TODO: no more board
    accessStatus.loadToRAM();

    // Now let's start the modem and set the system time fetched by the Modem network

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

    SystemManager::Init();

    // Drivers
    pn532Driver.emplace(ACTIVE_BOARD->nfcCs, ACTIVE_BOARD->nfcSpi);
    pn532Driver->begin();
    carKeyDriver.emplace(ACTIVE_BOARD->keyOpen, ACTIVE_BOARD->keyClose, ACTIVE_BOARD->keyPower.value_or(0),
                         ACTIVE_BOARD->hasKeyPower, keySequenceManager);
    carKeyDriver->begin();
    ledDriver.emplace(ACTIVE_BOARD->ledCount, ACTIVE_BOARD->led, NEO_GRB + NEO_KHZ800);
    ledDriver->begin();

    // Modules
    cardReaderModule.emplace(pn532Driver.value());
    cardReaderModule->begin();
    keyControlModule.emplace(carKeyDriver.value(),rfidsManager, accessStatus);
    ledModule.emplace(ledDriver.value());

    // Services
    cardReaderService.emplace(cardReaderModule.value());
    keyControlService.emplace(keyControlModule.value());
    ledService.emplace(ledModule.value());

    if constexpr (GIVE_CONNECTION_SPEED_ESTIMATE)
        queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::PerformConnectionSpeedTest);

    if constexpr (CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT)
        queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::DoFirmwareUpdateIfAvailable);
    else
        fileLog.infoln("Skipped firmware update check");

    // If there is no update we will continue with getting everything ready for reading NFC tags

    queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::DownloadRfidIfChanged);
    queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::DownloadGPSRfids);

    HelperUtils::logRAMUsage(fileLog, LoggingLevel::INFO);
    LittleFSHelper::logFilesystemsInformation();

    // Almost everything is done and the created log can be uploaded
    queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::UploadLog);

    // Power saving
    queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::DisconnectNetwork);
    queueModemTaskRxJob(ModemRxDataType::Command, ModemTaskCommand::SleepIfPossible);
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
