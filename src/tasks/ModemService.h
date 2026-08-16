#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <variant>
#include <Arduino.h>
#include <atomic>

#include "system/SystemThread.h"
#include "util/SequencePlayer.h"
#include "config/hw_config.h"
#include "domain/GPS.h"
#include "system/SystemManager.h"
#include "domain/AccessStatus.h"
#include "domain/ImeiStore.h"
#include "domain/LocalConfig.h"
#include "domain/RFIDs.h"
#include "logging/SwappableFile.h"

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

struct ModemTxMessage
{
    ModemTxDataType dataType;
    std::shared_ptr<ModemFlexibleTxPayload> payload;
    TickType_t queuedAt;
};

/// One unit of work for the modem task.
///
/// `deadline` is the tick after which running the command is no longer worth
/// doing, and the service drops it instead. That matters because the modem
/// serialises everything behind operations that can take minutes (an OTA, a log
/// upload): without a deadline, a poller like the GPS task fills the queue with
/// work that is stale by the time it runs, and every other caller — including
/// the card reader path — ends up waiting behind it.
struct ModemRequest
{
    /// For work that must run whenever the modem gets around to it.
    static constexpr TickType_t noDeadline = 0;

    ModemTaskCommand cmd = ModemTaskCommand::NONE;
    TickType_t deadline = noDeadline;

    [[nodiscard]] bool expired(const TickType_t now) const
    {
        return deadline != noDeadline && static_cast<int32_t>(now - deadline) >= 0;
    }
};

class ModemService : public SystemThread
{
public:
    ModemService(const LocalConfig& config, RFIDs& rfidsManager, SwappableFile& swLog, Modem& modem,
                 GPS& gps, ApiClient& api, ImeiStore& imeiStore)
        : SystemThread(SystemThreadId::ModemService, "MODEMSER", 8192, ThreadPriority::ModemService, 1),
          config(config), modem(modem), rfidsManager(rfidsManager),
          gps(gps), swLog(swLog), api(api), imeiStore(imeiStore)
    {
        SystemManager::RegisterThread(this);
    }

    ~ModemService() override;

    void OnCommand(SystemCommand cmd) override;

    /// How long a caller is willing to wait for room in the request queue. A full
    /// queue means the modem is badly backed up; callers get `false` and decide
    /// for themselves, rather than being parked indefinitely.
    static constexpr TickType_t defaultEnqueueTimeout = pdMS_TO_TICKS(100);

    /// Queues modem work. Never blocks indefinitely.
    ///
    /// `timeToLive` is how long the request stays worth running once queued; use
    /// `ModemRequest::noDeadline` for work that must always happen. Returns false
    /// if the request could not be queued, so time-critical callers (the unlock
    /// path) can carry on instead of stalling.
    bool sendRequest(ModemTaskCommand cmd, TickType_t timeToLive = ModemRequest::noDeadline,
                     TickType_t enqueueTimeout = defaultEnqueueTimeout);

    /// Waits for a reply of `dataType`. Replies of other types that arrive first
    /// are shelved for whoever is waiting on them. Returns nullptr on timeout.
    std::unique_ptr<ModemTxMessage> waitForSpecificMessage(ModemTxDataType dataType, TickType_t timeout);

    /// True while the service still has queued or in-flight work.
    [[nodiscard]] bool isWorkingOnTasks() const;
    [[nodiscard]] ModemState getCurrentState() const;

protected:
    void setup() override;
    void run() override;

private:
    /// Replies are dropped rather than allowed to block the modem task, which
    /// would stall every other caller behind it.
    static constexpr TickType_t replyEnqueueTimeout = pdMS_TO_TICKS(50);

    /// A shelved reply nobody claims within this long is discarded. Without it,
    /// reply types that currently have no waiter (a wakeup ack, for instance)
    /// accumulate for the whole 24h the device stays up.
    static constexpr TickType_t shelfLifetime = pdMS_TO_TICKS(30000);

    /// Idle wakeup rate of the request loop. Also bounds how long shutdown takes
    /// to be noticed.
    static constexpr TickType_t idlePollInterval = pdMS_TO_TICKS(500);

    std::atomic<bool> m_running = true;
    std::atomic<ModemState> currentSate = ModemState::NONE;

    /// Queued but not yet completed requests. Backs isWorkingOnTasks(), which
    /// other tasks use to tell whether the boot sequence has drained.
    std::atomic<uint32_t> m_outstandingRequests = 0;

    QueueHandle_t modemTaskTxQueue = xQueueCreate(20, sizeof(ModemTxMessage*));
    QueueHandle_t modemTaskRxQueue = xQueueCreate(20, sizeof(ModemRequest));

    /// Replies pulled off the queue by a task that wanted a different type.
    std::multimap<ModemTxDataType, ModemTxMessage*> m_shelvedMessages;
    std::mutex m_shelfMutex;

    const LocalConfig& config;
    Modem& modem;
    RFIDs& rfidsManager;
    GPS& gps;
    SwappableFile& swLog;
    ApiClient& api;
    ImeiStore& imeiStore;

    void handleRequest(ModemTaskCommand cmd);

    /// Hands a reply to whoever is waiting for it. Drops it if the queue is full.
    void publish(ModemTxDataType dataType, const ModemFlexibleTxPayload& payload);

    std::unique_ptr<ModemTxMessage> takeShelved(ModemTxDataType dataType);
    void shelve(std::unique_ptr<ModemTxMessage> msg);

    /// Must be called with m_shelfMutex held.
    void dropStaleShelvedLocked();
};
