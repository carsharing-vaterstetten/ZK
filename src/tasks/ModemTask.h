#pragma once
#include <array>
#include <atomic>
#include <mutex>
#include <optional>
#include <variant>
#include <Arduino.h>

#include "system/SystemThread.h"
#include "system/SystemManager.h"
#include "domain/GpsLog.h"
#include "domain/ImeiStore.h"
#include "domain/LocalConfig.h"
#include "domain/RFIDs.h"
#include "logging/SwappableFile.h"

/// Work the modem task can be asked to do, and — since it does one thing at a
/// time — also what it reports as its current state.
enum class ModemCommand : uint8_t
{
    None,
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

    /// Never sent as a request; reported by getCurrentState() when idle.
    Ready,
};

/// Values the modem task hands back to whoever asked for them.
enum class ModemResult : uint8_t
{
    Timestamp,
    WakeupSuccess,
    GPSData,
    UnixTimestamp,

    Count
};

struct ModemTimestamp
{
    int hour;
    int minute;
    int second;
};

using ModemPayload = std::variant<bool, ModemTimestamp, GPS_DATA_t, time_t>;

/// One unit of work for the modem task.
///
/// `deadline` is the tick after which the work is no longer worth doing and the
/// task drops it. The modem serialises everything behind operations that can run
/// for minutes (an OTA, a log upload), so without it a poller fills the queue
/// with stale work and every other caller waits behind it.
struct ModemRequest
{
    /// For work that must run whenever the modem gets around to it.
    static constexpr TickType_t noDeadline = 0;

    ModemCommand cmd = ModemCommand::None;
    TickType_t deadline = noDeadline;

    [[nodiscard]] bool expired(const TickType_t now) const
    {
        return deadline != noDeadline && static_cast<int32_t>(now - deadline) >= 0;
    }
};

class ModemTask : public SystemThread
{
public:
    ModemTask(const LocalConfig& config, RFIDs& rfidsManager, SwappableFile& swLog, Modem& modem,
              GpsLog& gps, ApiClient& api, ImeiStore& imeiStore)
        : SystemThread(SystemThreadId::ModemTask, "MODEMTSK", 8192, ThreadPriority::ModemTask, 1),
          config(config), modem(modem), rfidsManager(rfidsManager),
          gps(gps), swLog(swLog), api(api), imeiStore(imeiStore)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

    /// A full queue means the modem is badly backed up; callers get `false` and
    /// decide for themselves rather than being parked indefinitely.
    static constexpr TickType_t defaultEnqueueTimeout = pdMS_TO_TICKS(100);

    /// Queues modem work. Never blocks indefinitely. `timeToLive` is how long
    /// the request stays worth running; use noDeadline for work that must happen.
    bool sendRequest(ModemCommand cmd, TickType_t timeToLive = ModemRequest::noDeadline,
                     TickType_t enqueueTimeout = defaultEnqueueTimeout);

    /// Waits for a result, consuming it. Returns nullopt on timeout.
    ///
    /// One slot per type, not a shared queue, so it does not matter which task
    /// asked: StartupTask requests the timestamp that RestartTask consumes.
    std::optional<ModemPayload> waitFor(ModemResult type, TickType_t timeout);

    /// True while there is queued or in-flight work.
    [[nodiscard]] bool hasPendingWork() const { return m_outstandingRequests > 0; }
    [[nodiscard]] ModemCommand getCurrentState() const { return m_currentState; }

protected:
    void setup() override;
    void run() override;

private:
    /// Idle wakeup rate of the request loop. Also bounds how long shutdown takes
    /// to be noticed.
    static constexpr TickType_t idlePollInterval = pdMS_TO_TICKS(500);

    static constexpr EventBits_t bitFor(const ModemResult type)
    {
        return static_cast<EventBits_t>(1) << static_cast<uint8_t>(type);
    }

    std::atomic<ModemCommand> m_currentState = ModemCommand::None;

    /// Queued but not yet completed requests. Backs hasPendingWork().
    std::atomic<uint32_t> m_outstandingRequests = 0;

    QueueHandle_t m_requests = xQueueCreate(20, sizeof(ModemRequest));

    /// Latest value per result type. A result nobody waits for is simply
    /// overwritten by the next one, so nothing accumulates.
    std::array<std::optional<ModemPayload>, static_cast<size_t>(ModemResult::Count)> m_results;
    std::mutex m_resultsMutex;
    EventGroupHandle_t m_resultReady = xEventGroupCreate();

    const LocalConfig& config;
    Modem& modem;
    RFIDs& rfidsManager;
    GpsLog& gps;
    SwappableFile& swLog;
    ApiClient& api;
    ImeiStore& imeiStore;

    void handleRequest(ModemCommand cmd);

    void publish(ModemResult type, ModemPayload payload);

    /// Consumes the slot if it holds anything.
    std::optional<ModemPayload> take(ModemResult type);

    /// Both retry until the modem returns something sane; the SIM7000 reports
    /// placeholder values for a while after it comes up.
    void publishUnixTime();
    void publishNetworkTime();

    /// Retries while the answer doesn't look like an IMEI (15 digits) — a stray
    /// unsolicited line from the modem can otherwise be mistaken for one. Stores
    /// whatever it has after exhausting retries even if still implausible: this
    /// is the credential every authenticated request blocks on, and leaving it
    /// unset would turn a bad IMEI (recoverable — the device still unlocks cars,
    /// only backend sync fails) into every such request hanging forever instead.
    void publishImei();
};
