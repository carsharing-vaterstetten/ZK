#pragma once
#include <memory>
#include <variant>
#include <Arduino.h>
#include <atomic>

#include "SystemThread.h"
#include "abstract/PinSequencePlayer.h"
#include "config/hw_config.h"
#include "../modules/GPS.h"
#include "logic/SystemManager.h"
#include "shared/AccessStatus.h"
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
};

enum class ModemTxDataType
{
    IMEI,
    Timestamp,
    WakeupSuccess,
    GPSData,
    AccessControlSequences,
};

enum class ModemRxDataType
{
    Command,
};

typedef struct
{
    std::vector<SequencePoint> openSequence;
    std::vector<SequencePoint> closeSequence;
} AccessControlSequences;

typedef struct
{
    int hour;
    int minute;
    int second;
} ModemTimestamp;

using ModemFlexibleTxPayload = std::variant<String, bool, ModemTimestamp, GPS_DATA_t, AccessControlSequences>;
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
                 GPS& gps) : SystemThread(SystemThreadId::ModemService, "MODEMSER", 8192, 4), board(board),
                             config(config), accessStatus(accessStatus), modem(modem), rfidsManager(rfidsManager),
                             gps(gps), swLog(swLog)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

    void queueModemTaskRxJob(ModemRxDataType dataType, ModemFlexibleRxPayload payload);
    void modemFinishTasksAndStop();
    ModemTxMessage* waitForSpecificModemMessage(ModemTxDataType dataType);

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    QueueHandle_t modemTaskTxQueue = xQueueCreate(10, sizeof(ModemTxMessage*));
    QueueHandle_t modemTaskRxQueue = xQueueCreate(10, sizeof(ModemRxMessage*));

    const BoardConfig& board;
    const LocalConfig& config;
    const AccessStatus& accessStatus;
    Modem& modem;
    RFIDs& rfidsManager;
    GPS& gps;
    SwappableFile& swLog;

    std::optional<ApiClient> api;
};
