#pragma once
#include <memory>
#include <variant>
#include <Arduino.h>
#include <atomic>

#include "SystemThread.h"
#include "abstract/SequencePlayer.h"
#include "config/hw_config.h"
#include "../modules/GPS.h"
#include "logic/SystemManager.h"
#include "shared/AccessStatus.h"
#include "shared/ImeiStore.h"
#include "shared/LocalConfig.h"
#include "shared/RFIDs.h"
#include "shared/SwappableFile.h"

#define MODEM_TASK_COMMANDS InitializeModem, PerformConnectionSpeedTest, DoFirmwareUpdateIfAvailable, DownloadRfidIfChanged, DownloadGPSRfids, UploadLog, DisconnectNetwork, SleepIfPossible, ConnectNetwork, Wakeup, EnableGPS, GetGPSData, UploadGPSData, GetAccessControlSequences, GetUnixTime, GetTimestamp, GetImei, NONE

enum class ModemTaskCommand
{
    MODEM_TASK_COMMANDS
};

enum class ModemState
{
    MODEM_TASK_COMMANDS, // order must be preserved! See conversion function
    READY,
};

inline ModemState modemCmdToState(ModemTaskCommand cmd)
{
    return static_cast<ModemState>(cmd);
}

enum class ModemTxDataType
{
    IMEI,
    Timestamp,
    WakeupSuccess,
    GPSData,
    AccessControlSequences,
    UnixTimestamp,
};

typedef struct
{
    int hour;
    int minute;
    int second;
} ModemTimestamp;

using ModemFlexibleTxPayload = std::variant<String, bool, ModemTimestamp, GPS_DATA_t, time_t>;

typedef struct ModemTxMessage
{
    ModemTxDataType dataType;
    std::shared_ptr<ModemFlexibleTxPayload> payload;
};

class ModemService : public SystemThread
{
public:
    ModemService(const BoardConfig& board, const LocalConfig& config, RFIDs& rfidsManager,
                 SwappableFile& swLog, const AccessStatus& accessStatus, Modem& modem,
                 GPS& gps, ApiClient& api, ImeiStore& imeiStore) : SystemThread(SystemThreadId::ModemService,
                                                                       "MODEMSER", 8192, ThreadPriority::ModemService,
                                                                       1), board(board),
                                                                   config(config), accessStatus(accessStatus),
                                                                   modem(modem), rfidsManager(rfidsManager),
                                                                   gps(gps), swLog(swLog), api(api),
                                                                   imeiStore(imeiStore)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

    void sendRequest(ModemTaskCommand cmd) const;
    ModemTxMessage* waitForSpecificMessage(ModemTxDataType dataType, TickType_t timeout);

    bool isWorkingOnTasks();
    ModemState getCurrentState();

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;
    std::atomic<bool> workingOnTasks = true;
    std::atomic<bool> lowPowerRequested = false;
    std::atomic<ModemState> currentSate = ModemState::NONE;
    std::atomic<TickType_t> lastReceivedMessageTime = 0;

    QueueHandle_t modemTaskTxQueue = xQueueCreate(20, sizeof(ModemTxMessage*));
    QueueHandle_t modemTaskRxQueue = xQueueCreate(20, sizeof(ModemTaskCommand));

    const BoardConfig& board;
    const LocalConfig& config;
    const AccessStatus& accessStatus;
    Modem& modem;
    RFIDs& rfidsManager;
    GPS& gps;
    SwappableFile& swLog;
    ApiClient& api;
    ImeiStore& imeiStore;

    void sendAnswer(ModemTxDataType dataType, const ModemFlexibleTxPayload& payload) const;
};
