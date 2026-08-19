#pragma once

#include <FS.h>
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB
#include <TinyGsmClient.h>

#include "net/ApiStreams.h"
#include "domain/GpsLog.h"
#include "hal/ModemHardware.h"

#define BASE_UPLOAD_RESULTS SUCCESS, FAILED

class ApiClient;

enum class UploadAndRetryResult
{
    BASE_UPLOAD_RESULTS,
    SUCCESS_AFTER_RETRYING,
};

enum class UploadFileAndRetryResult
{
    BASE_UPLOAD_RESULTS,
    FILE_IS_EMPTY,
    FILE_DOES_NOT_EXIST,
    FAILED_TO_OPEN_FILE,
    SUCCESS_AFTER_RETRYING,
};

enum class SleepRequestResult
{
    FailedBecauseModemIsStillInUse,
    Failed,
    Success,
    AlreadySleeping,
};

class Modem
{
protected:
    bool modemIsAwake = false, gpsIsEnabled = false;
    bool successfulHotstart = false;
    ulong serialBaud;

    HardwareSerial& serial;
    TinyGsmSim7000& gsmModem;

    const char *gprsUser = "", *gprsPassword = "", *apn = "";

    const ModemHardware& driver;

    bool beginSleep();
    std::tuple<bool, ulong> autoBaud(uint32_t timeoutMs);
    bool connectGPRSAndNetwork(uint retries = 2) const;
    bool beginHot(const char* simPin);
    bool beginCold(const char* simPin, uint retries);
    bool finishInit(const char* simPin, ulong detectedBaud) const;
    void forcePowerCycle() const;
    bool waitForRDY(uint32_t timeout_ms);

    /// Discards bytes already sitting in the UART's RX buffer. Called before any
    /// command whose response is read by "next line", rather than by matching a
    /// specific prefix — those are the ones an unconsumed unsolicited line (e.g.
    /// "+CPIN: READY", which the SIM7000 can emit asynchronously well after
    /// gsmModem.init() already returned) can get mistaken for the answer to.
    void drainStaleInput() const;
    static ApiResponse uploadData(ApiClient& api, const char* endpoint, Stream& stream, size_t streamLen);
    static UploadAndRetryResult uploadDataAndRetry(ApiClient& api, const char* endpoint, Stream& stream,
                                                   size_t streamLen, uint retries);

public:
    Modem(TinyGsmSim7000& gsmModem, HardwareSerial& hwSerial, ulong serialBaud, const ModemHardware& driver);

    void powerOn() const;
    SleepRequestResult requestSleep();

    bool connect(const char* simPin, const char* user, const char* password, const char* netApn, uint retries = 2);
    bool ensureNetworkConnection(uint maxRetries = 2) const;
    void wakeup();
    bool wakeupAndWait(uint32_t timeoutMs = 10000);
    static UploadFileAndRetryResult uploadFileAndDelete(ApiClient& api, const char* endpoint, File& f,
                                                        bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);
    static UploadFileAndRetryResult uploadFileAndDelete(ApiClient& api, const char* endpoint,
                                                        const char* filePath,
                                                        bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);

    bool disconnectNetwork() const;
    bool enableGPS();

    bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute, int* second, float* timezone) const
    {
        return gsmModem.getNetworkTime(year, month, day, hour, minute, second, timezone);
    }

    [[nodiscard]] time_t getUnixTimestamp() const;
    bool getGPS(GPS_DATA_t& out) const;

    [[nodiscard]] String getIMEI() const;
};
