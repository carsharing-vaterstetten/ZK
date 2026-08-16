#pragma once

#include "config/hw_config.h"
#include "domain/AccessStatus.h"
#include "domain/GpsLog.h"
#include "domain/ImeiStore.h"
#include "domain/LocalConfig.h"
#include "domain/RFIDs.h"
#include "domain/TripTracker.h"
#include "hal/CarKey.h"
#include "hal/KeySequences.h"
#include "hal/Led.h"
#include "hal/Modem.h"
#include "hal/ModemHardware.h"
#include "hal/NfcReader.h"
#include "net/ApiClient.h"
#include "tasks/AccessControlTask.h"
#include "tasks/CardReaderTask.h"
#include "tasks/GpsTask.h"
#include "tasks/KeyControlTask.h"
#include "tasks/LedSchedulerTask.h"
#include "tasks/ModemTask.h"
#include "tasks/RestartTask.h"
#include "tasks/StartupTask.h"
#include "tasks/SystemWatchTask.h"

/// The whole object graph, held by value.
///
/// Member declaration order is construction order, so the driver -> hardware ->
/// task dependency chain is enforced by the compiler instead of by the order of
/// a list of .emplace() calls. Constructors only wire things together; anything
/// that touches hardware happens in begin().
class Device
{
public:
    Device(const BoardConfig& board, LocalConfig config);

    /// Brings up hardware, loads persisted state, then starts every task.
    void begin();

private:
    const BoardConfig& m_board;
    LocalConfig m_config;

    // Domain state. No dependencies on hardware.
    TripTracker m_tripTracker;
    RFIDs m_rfids;
    KeySequences m_keySequences;
    AccessStatus m_accessStatus;
    ImeiStore m_imeiStore;

    // Vendor drivers.
    SPIClass m_nfcSpi;
    Adafruit_PN532 m_pn532;
    Adafruit_NeoPixel m_neoPixel;
    HardwareSerial& m_modemSerial;
    ModemHardware m_modemHardware;
    TinyGsm m_gsm;
    TinyGsmClient m_gsmClient;
    WdClient m_httpClient;

    // Hardware wrappers.
    NfcReader m_nfcReader;
    CarKey m_carKey;
    Led m_led;
    Modem m_modem;

    // Domain services that need hardware.
    GpsLog m_gpsLog;
    ApiClient m_api;

    // Tasks. Each registers itself with SystemManager on construction.
    LedSchedulerTask m_ledScheduler;
    CardReaderTask m_cardReader;
    KeyControlTask m_keyControl;
    ModemTask m_modemTask;
    AccessControlTask m_accessControl;
    GpsTask m_gps;
    RestartTask m_restart;
    StartupTask m_startup;
    SystemWatchTask m_systemWatch;
};
