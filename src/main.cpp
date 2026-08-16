#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB
#include <esp32-hal.h>

#include "config/Config.h"
#include "hal/Modem.h"
#include "hal/NfcReader.h"
#include "util/HelperUtils.h"
#include "esp_log.h"
#include "net/ApiClient.h"
#include "logging/Loggers.h"
#include "domain/GpsLog.h"
#include "domain/TripTracker.h"
#include "hal/KeySequences.h"
#include "domain/LocalConfig.h"
#include "domain/RFIDs.h"
#include "logging/SwappableFile.h"
#include "util/Files.h"
#include "system/SystemManager.h"
#include "config/Backend.h"
#include "config/hw_config.h"
#include "config/Intern.h"
#include "esp32/rom/rtc.h"
#include "config/Config.h"
#include "config/user_config.h"
#include "tasks/CardReaderTask.h"
#include "domain/ImeiStore.h"
#include "tasks/AccessControlTask.h"
#include "tasks/GpsTask.h"
#include "tasks/StartupTask.h"
#include "tasks/RestartTask.h"
#include "tasks/SystemWatchTask.h"

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

// Dont know what these are
std::optional<ApiClient> apiDriver;
std::optional<TinyGsm> modemDriver;
std::optional<TinyGsmClient> gsmClient;
std::optional<WdClient> wdClient;

// Shared
std::optional<LocalConfig> config;
TripTracker tripTracker{};
RFIDs rfidsManager{"/rfids.bin", "/tmp_rfids.bin", "/rfids_gps_consent.bin", "/tmp_rfids_gps_consent.bin"};
KeySequences keySequences;
AccessStatus accessStatus{"AccCtrl v1", rfidsManager};
ImeiStore imeiStore{};

// Drivers
std::optional<Adafruit_PN532> pn532Driver;
std::optional<CarKey> carKey;
std::optional<Adafruit_NeoPixel> ledDriver;
std::optional<ModemHardware> modemHardware;
std::optional<HardwareSerial> modemSerialDriver;
std::optional<SPIClass> nfcSpiDriver;

// Modules
std::optional<NfcReader> nfcReader;
std::optional<KeyControl> keyControlModule;
std::optional<Led> ledModule;
std::optional<Modem> modemModule;
std::optional<GpsLog> gpsModule;

// Services
std::optional<CardReaderTask> cardReaderTask;
std::optional<KeyControlTask> keyControlTask;
std::optional<ModemTask> modemTask;

// Tasks
std::optional<AccessControlTask> accessControlTask;
std::optional<GpsTask> gpsTask;
std::optional<RestartTask> restartTask;
std::optional<StartupTask> startupTask;
std::optional<SystemWatchTask> watchTask;
std::optional<LedSchedulerTask> ledSchedulerTask;

int espLogHandler(const char* fmt, const va_list args)
{
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    logger.errorln(buf);
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
        serialLogger.warningln("No or outdated config found. Requesting new config.");
        config.emplace(HelperUtils::requestConfig());
        const bool configSaveSuccess = StorableConfig{config.value(), CONFIG_PREFS_NAME}.save();
        logger.logInfoOrErrorln(configSaveSuccess, "Successfully saved config", "Failed to save config");
    }
}

void setup()
{
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
    esp_task_wdt_deinit();

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
        logger.addOutputSink(Serial, "", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
        serialLogger.addOutputSink(Serial, "Serial", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
    }

    // Mount filesystems
    const bool flashInitSuccess = LittleFS.begin(true);
    serialLogger.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");

    LittleFS.remove("/gps.bin"); // Delete old file

    swLog.begin(true);
    logger.addOutputSink(swLog, "", true, false, FLASH_LOGGING_LEVEL, true, true);

    // Logging to files is now possible
    esp_log_set_vprintf(&espLogHandler); // Redirect ESP logs to file
    logger.infoln("Running firmware version " FIRMWARE_VERSION);
    const RESET_REASON cpu0ResetReason = rtc_get_reset_reason(0);
    logger.infoln("CPU0 reset reason: " + HelperUtils::getResetReasonHumanReadable(cpu0ResetReason));
    logger.infoln("CPU1 reset reason: " + HelperUtils::getResetReasonHumanReadable(rtc_get_reset_reason(1)));

    // Now that critical system hardware has been initialized when can begin initializing external hardware
    // First we start the LED to communicate the system status

    rfidsManager.loadFromFileToRam();
    rfidsManager.loadFromGpsFileToRam();
    keySequences.loadSequenceInRAM(*ACTIVE_BOARD); // TODO: no more board
    accessStatus.begin();
    accessStatus.loadToRAM();

    if (accessStatus.isLoggedIn())
        logger.infoln("RFID UID " + String(accessStatus.getLoggedInUID().value_or(0), 16) + " is logged in");

    if constexpr (USE_DEFAULT_CONFIG)
    {
        config.emplace(DEFAULT_CONFIG_APN, DEFAULT_CONFIG_GPRS_USER, DEFAULT_CONFIG_GPRS_PASSWORD,
                       DEFAULT_CONFIG_SERVER, DEFAULT_CONFIG_PORT, DEFAULT_CONFIG_PASSWORD, DEFAULT_SIM_PIN);
        logger.infoln("Using default config");
    }
    else
        loadConfig(); // We need the config for the Modem

    logger.infoln("Loaded config: " + config->toString());

    SystemManager::Init();

    // Drivers
    nfcSpiDriver.emplace(ACTIVE_BOARD->nfcSpi);
    nfcSpiDriver->begin(ACTIVE_BOARD->nfcClk, ACTIVE_BOARD->nfcMiso, ACTIVE_BOARD->nfcMosi, ACTIVE_BOARD->nfcCs);
    pn532Driver.emplace(ACTIVE_BOARD->nfcCs, &nfcSpiDriver.value());
    pn532Driver->begin();
    carKey.emplace(ACTIVE_BOARD->keyOpen, ACTIVE_BOARD->keyClose, ACTIVE_BOARD->keyPower.value_or(0),
                         ACTIVE_BOARD->hasKeyPower, keySequences);
    carKey->begin();
    ledDriver.emplace(ACTIVE_BOARD->ledCount, ACTIVE_BOARD->led, NEO_GRB + NEO_KHZ800);
    ledDriver->begin();
    modemSerialDriver.emplace(Serial1);
    modemSerialDriver->setRxBufferSize(2048);
    modemSerialDriver->begin(MODEM_SERIAL_BAUD, SERIAL_8N1, ACTIVE_BOARD->modemRx, ACTIVE_BOARD->modemTx);
    modemHardware.emplace(*ACTIVE_BOARD);
    modemHardware->begin();

    // dont know
    modemDriver.emplace(modemSerialDriver.value());
    gsmClient.emplace(modemDriver.value());
    wdClient.emplace(gsmClient.value(), config->server, config->serverPort);
    apiDriver.emplace(wdClient.value(), imeiStore, config->serverPassword);

    // Modules
    nfcReader.emplace(pn532Driver.value());
    keyControlModule.emplace(carKey.value());
    ledModule.emplace(ledDriver.value());
    modemModule.emplace(modemDriver.value(), modemSerialDriver.value(), MODEM_SERIAL_BAUD, modemHardware.value());
    gpsModule.emplace("/gps.bin", GPS_FILE_UPLOAD_ENDPOINT);
    gpsModule->begin();

    // Services
    cardReaderTask.emplace(nfcReader.value());
    keyControlTask.emplace(keyControlModule.value());
    modemTask.emplace(config.value(), rfidsManager, swLog, modemModule.value(),
                         gpsModule.value(), apiDriver.value(), imeiStore);

    // Tasks
    ledSchedulerTask.emplace(ledModule.value());
    accessControlTask.emplace(rfidsManager, tripTracker, keyControlTask.value(), accessStatus, ledSchedulerTask.value(),
                   modemTask.value(), cardReaderTask.value());
    gpsTask.emplace(accessStatus, modemTask.value(), tripTracker, gpsModule.value());
    restartTask.emplace(TARGET_TIME_FOR_ESP_RESTART, modemTask.value());
    startupTask.emplace(modemTask.value(), accessStatus, ledSchedulerTask.value(), apiDriver.value());
    watchTask.emplace();

    SystemManager::Start();

    // StartupTask owns the boot sequence end to end
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
