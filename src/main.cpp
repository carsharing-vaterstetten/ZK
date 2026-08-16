#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB
#include <esp32-hal.h>

#include "config/Config.h"
#include "hal/Modem.h"
#include "hal/NFCCardReader.h"
#include "util/HelperUtils.h"
#include "esp_log.h"
#include "net/Api.h"
#include "logging/Loggers.h"
#include "domain/GPS.h"
#include "domain/GPSAlg.h"
#include "hal/KeySequenceManager.h"
#include "domain/LocalConfig.h"
#include "domain/RFIDs.h"
#include "logging/SwappableFile.h"
#include "util/LittleFSHelper.h"
#include "system/SystemManager.h"
#include "config/Backend.h"
#include "config/hw_config.h"
#include "config/Intern.h"
#include "esp32/rom/rtc.h"
#include "config/Config.h"
#include "config/user_config.h"
#include "tasks/CardReaderService.h"
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
GPSAlg gpsAlg{};
RFIDs rfidsManager{"/rfids.bin", "/tmp_rfids.bin", "/rfids_gps_consent.bin", "/tmp_rfids_gps_consent.bin"};
KeySequenceManager keySequenceManager;
AccessStatus accessStatus{"AccCtrl v1", rfidsManager};
ImeiStore imeiStore{};

// Drivers
std::optional<Adafruit_PN532> pn532Driver;
std::optional<CarKeyDriver> carKeyDriver;
std::optional<Adafruit_NeoPixel> ledDriver;
std::optional<ModemHardwareDriver> modemHardwareDriver;
std::optional<HardwareSerial> modemSerialDriver;
std::optional<SPIClass> nfcSpiDriver;

// Modules
std::optional<NFCCardReader> cardReaderModule;
std::optional<KeyControl> keyControlModule;
std::optional<StatefulLed> ledModule;
std::optional<Modem> modemModule;
std::optional<GPS> gpsModule;

// Services
std::optional<CardReaderService> cardReaderService;
std::optional<KeyControlService> keyControlService;
std::optional<ModemService> modemService;

// Tasks
std::optional<AccessControlTask> acTask;
std::optional<GPSTask> gpsTask;
std::optional<RestartTask> restartTask;
std::optional<StartupTask> startupTask;
std::optional<SystemWatchTask> watchTask;
std::optional<LedSchedulerTask> ledSchedularTask;

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
        config.emplace(HelperUtils::requestConfig());
        const bool configSaveSuccess = StorableConfig{config.value(), CONFIG_PREFS_NAME}.save();
        fileLog.logInfoOrErrorln(configSaveSuccess, "Successfully saved config", "Failed to save config");
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
        fileLog.addOutputSink(Serial, "", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
        serialOnlyLog.addOutputSink(Serial, "Serial", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
    }

    // Mount filesystems
    const bool flashInitSuccess = LittleFS.begin(true);
    serialOnlyLog.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                           "Flash initialization failed");

    LittleFS.remove("/gps.bin"); // Delete old file

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

    rfidsManager.loadFromFileToRam();
    rfidsManager.loadFromGpsFileToRam();
    keySequenceManager.loadSequenceInRAM(*ACTIVE_BOARD); // TODO: no more board
    accessStatus.begin();
    accessStatus.loadToRAM();

    if (accessStatus.isLoggedIn())
        fileLog.infoln("RFID UID " + String(accessStatus.getLoggedInUID().value_or(0), 16) + " is logged in");

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
    nfcSpiDriver.emplace(ACTIVE_BOARD->nfcSpi);
    nfcSpiDriver->begin(ACTIVE_BOARD->nfcClk, ACTIVE_BOARD->nfcMiso, ACTIVE_BOARD->nfcMosi, ACTIVE_BOARD->nfcCs);
    pn532Driver.emplace(ACTIVE_BOARD->nfcCs, &nfcSpiDriver.value());
    pn532Driver->begin();
    carKeyDriver.emplace(ACTIVE_BOARD->keyOpen, ACTIVE_BOARD->keyClose, ACTIVE_BOARD->keyPower.value_or(0),
                         ACTIVE_BOARD->hasKeyPower, keySequenceManager);
    carKeyDriver->begin();
    ledDriver.emplace(ACTIVE_BOARD->ledCount, ACTIVE_BOARD->led, NEO_GRB + NEO_KHZ800);
    ledDriver->begin();
    modemSerialDriver.emplace(Serial1);
    modemSerialDriver->setRxBufferSize(2048);
    modemSerialDriver->begin(MODEM_SERIAL_BAUD, SERIAL_8N1, ACTIVE_BOARD->modemRx, ACTIVE_BOARD->modemTx);
    modemHardwareDriver.emplace(*ACTIVE_BOARD);
    modemHardwareDriver->begin();

    // dont know
    modemDriver.emplace(modemSerialDriver.value());
    gsmClient.emplace(modemDriver.value());
    wdClient.emplace(gsmClient.value(), config->server, config->serverPort);
    apiDriver.emplace(wdClient.value(), imeiStore, config->serverPassword);

    // Modules
    cardReaderModule.emplace(pn532Driver.value());
    keyControlModule.emplace(carKeyDriver.value());
    ledModule.emplace(ledDriver.value());
    modemModule.emplace(modemDriver.value(), modemSerialDriver.value(), MODEM_SERIAL_BAUD, modemHardwareDriver.value());
    gpsModule.emplace("/gps.bin", GPS_FILE_UPLOAD_ENDPOINT);
    gpsModule->begin();

    // Services
    cardReaderService.emplace(cardReaderModule.value());
    keyControlService.emplace(keyControlModule.value());
    modemService.emplace(config.value(), rfidsManager, swLog, modemModule.value(),
                         gpsModule.value(), apiDriver.value(), imeiStore);

    // Tasks
    ledSchedularTask.emplace(ledModule.value());
    acTask.emplace(rfidsManager, gpsAlg, keyControlService.value(), accessStatus, ledSchedularTask.value(),
                   modemService.value(), cardReaderService.value());
    gpsTask.emplace(accessStatus, modemService.value(), gpsAlg, gpsModule.value());
    restartTask.emplace(TARGET_TIME_FOR_ESP_RESTART, modemService.value());
    startupTask.emplace(modemService.value(), accessStatus, ledSchedularTask.value(), apiDriver.value());
    watchTask.emplace();

    SystemManager::Start();

    // StartupTask owns the boot sequence end to end
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
