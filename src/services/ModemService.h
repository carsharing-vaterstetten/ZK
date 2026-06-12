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

enum class ModemTaskCommand
{
    ReadData,
    WriteData,
    InitializeModem,
    PerformConnectionSpeedTest,
    DoFirmwareUpdateIfAvailable,
    DownloadRfidIfChanged,
    DownloadGPSRfids,
    UploadLog,
    DisconnectNetwork,
    SleepIfPossible,
    ConnectNetwork,
    Wakeup,
    EnableGPS,
    GetGPSData,
    UploadGPSData,
    GetAccessControlSequences,
    GetUnixTime,
    GetTimestamp,
    GetImei,
};

enum class ModemTxDataType
{
    IMEI,
    Timestamp,
    WakeupSuccess,
    GPSData,
    AccessControlSequences,
    UnixTimestamp,
};

enum class ModemRxDataType
{
    Command,
};

typedef struct
{
    int hour;
    int minute;
    int second;
} ModemTimestamp;

using ModemFlexibleTxPayload = std::variant<String, bool, ModemTimestamp, GPS_DATA_t, time_t>;
using ModemFlexibleRxPayload = std::variant<ModemTaskCommand>;

typedef struct ModemTxMessage
{
    ModemTxDataType dataType;
    std::shared_ptr<ModemFlexibleTxPayload> payload;
};

typedef struct ModemRxMessage
{
    ModemRxDataType dataType;
    std::shared_ptr<ModemFlexibleRxPayload> payload;
};


class ModemService : public SystemThread
{
public:
    ModemService(const BoardConfig& board,
                 const LocalConfig& config,
                 RFIDs& rfidsManager,
                 SwappableFile& swLog, const AccessStatus& accessStatus, Modem& modem,
                 GPS& gps, ApiClient& api, ImeiStore& imeiStore) : SystemThread(SystemThreadId::ModemService, "MODEMSER", 8192, ThreadPriority::ModemService), board(board),
                             config(config), accessStatus(accessStatus), modem(modem), rfidsManager(rfidsManager),
                             gps(gps), swLog(swLog), api(api), imeiStore(imeiStore)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

    void sendMessage(ModemRxDataType dataType, const ModemFlexibleRxPayload& payload);
    ModemTxMessage* waitForSpecificMessage(ModemTxDataType dataType, TickType_t timeout);

    bool isWorkingOnTasks();

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;
    std::atomic<bool> workingOnTasks = false;

    QueueHandle_t modemTaskTxQueue = xQueueCreate(20, sizeof(ModemTxMessage*));
    QueueHandle_t modemTaskRxQueue = xQueueCreate(20, sizeof(ModemRxMessage*));

    const BoardConfig& board;
    const LocalConfig& config;
    const AccessStatus& accessStatus;
    Modem& modem;
    RFIDs& rfidsManager;
    GPS& gps;
    SwappableFile& swLog;
    ApiClient& api;
    ImeiStore& imeiStore;

    void sendMessage(ModemTxDataType dataType, const ModemFlexibleTxPayload& payload);
};
