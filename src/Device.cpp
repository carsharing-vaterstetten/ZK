#include "Device.h"

#include "config/Backend.h"
#include "config/Intern.h"
#include "config/user_config.h"
#include "logging/Loggers.h"
#include "system/SystemManager.h"

Device::Device(const BoardConfig& board, LocalConfig config)
    : m_board(board),
      m_config(std::move(config)),
      m_rfids("/rfids.bin", "/tmp_rfids.bin", "/rfids_gps_consent.bin", "/tmp_rfids_gps_consent.bin"),
      m_accessStatus("AccCtrl v1", m_rfids),
      m_nfcSpi(board.nfcSpi),
      m_pn532(board.nfcCs, &m_nfcSpi),
      m_neoPixel(board.ledCount, board.led, NEO_GRB + NEO_KHZ800),
      m_modemSerial(Serial1),
      m_modemHardware(board),
      m_gsm(m_modemSerial),
      m_gsmClient(m_gsm),
      m_httpClient(m_gsmClient, m_config.server, m_config.serverPort),
      m_nfcReader(m_pn532),
      m_carKey(board.keyOpen, board.keyClose, board.keyPower.value_or(0), board.hasKeyPower, m_keySequences),
      m_led(m_neoPixel),
      m_modem(m_gsm, m_modemSerial, MODEM_SERIAL_BAUD, m_modemHardware),
      m_gpsLog("/gps.bin", GPS_FILE_UPLOAD_ENDPOINT),
      m_api(m_httpClient, m_imeiStore, m_config.serverPassword),
      m_ledScheduler(m_led),
      m_cardReader(m_nfcReader),
      m_keyControl(m_carKey),
      m_modemTask(m_config, m_rfids, swLog, m_modem, m_gpsLog, m_api, m_imeiStore),
      m_accessControl(m_rfids, m_tripTracker, m_keyControl, m_accessStatus, m_ledScheduler, m_modemTask, m_cardReader),
      m_gps(m_accessStatus, m_modemTask, m_tripTracker, m_gpsLog),
      m_restart(TARGET_TIME_FOR_ESP_RESTART, m_modemTask),
      m_startup(m_modemTask, m_accessStatus, m_ledScheduler, m_api),
      m_systemWatch()
{
}

void Device::begin()
{
    logger.infoln("Loaded config: " + m_config.toString());

    m_nfcSpi.begin(m_board.nfcClk, m_board.nfcMiso, m_board.nfcMosi, m_board.nfcCs);
    m_pn532.begin();
    m_carKey.begin();
    m_neoPixel.begin();
    m_modemSerial.setRxBufferSize(2048);
    m_modemSerial.begin(MODEM_SERIAL_BAUD, SERIAL_8N1, m_board.modemRx, m_board.modemTx);
    m_modemHardware.begin();
    m_gpsLog.begin();

    m_rfids.loadFromFileToRam();
    m_rfids.loadFromGpsFileToRam();
    m_keySequences.loadSequenceInRAM(m_board);
    m_accessStatus.begin();
    m_accessStatus.loadToRAM();

    if (m_accessStatus.isLoggedIn())
        logger.infoln("RFID UID " + String(m_accessStatus.getLoggedInUID().value_or(0), 16) + " is logged in");

    SystemManager::Init();
    SystemManager::Start(); // StartupTask drives the boot sequence from here
}
