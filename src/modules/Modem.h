#pragma once

#include <FS.h>
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB
#include <TinyGsmClient.h>

#include "../abstract/ApiStreams.h"
#include "GPS.h"
#include "drivers/modem.h"

#define BASE_UPLOAD_RESULTS SUCCESS, FAILED

class ApiClient;

enum class UploadResult
{
    BASE_UPLOAD_RESULTS,
};

enum class UploadFileResult
{
    BASE_UPLOAD_RESULTS,
    FILE_IS_EMPTY
};

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

enum class PowerOffMethod
{
    UartCommand,
    PwrKey,
};

class Modem
{
protected:
    bool modemIsAwake = false, gpsIsEnabled = false;
    bool successfulHotstart;
    ulong serialBaud;

    HardwareSerial& serial;
    TinyGsmSim7000& gsmModem;

    const char *gprsUser = "", *gprsPassword = "", *apn = "";

    static constexpr uint32_t modemPowerOnPulseWidthMs = 1000, modemPowerOffPulseWidthMs = 1300;

    const ModemHardwareDriver& driver;

    bool beginSleep();
    std::tuple<bool, ulong> autoBaud(uint32_t timeoutMs);
    bool connectGPRSAndNetwork(uint retries = 2) const;
    bool beginHot(const char* simPin);
    bool beginCold(const char* simPin, uint retries);
    bool finishInit(const char* simPin, ulong detectedBaud) const;
    void forcePowerCycle() const;

public:
    Modem(TinyGsmSim7000& gsmModem, HardwareSerial& hwSerial, ulong serialBaud, const ModemHardwareDriver& driver);

    void powerOff(PowerOffMethod method) const;
    void powerOn() const;
    SleepRequestResult requestSleep();

    bool connect(const char* simPin, const char* user, const char* password, const char* netApn, uint retries = 2);
    bool ensureNetworkConnection(uint maxRetries = 2) const;
    void wakeup();
    bool wakeupAndWait(uint32_t timeoutMs = 10000);
    static ApiResponse uploadData(const ApiClient& api, const char* endpoint, Stream& stream, size_t streamLen);
    static UploadAndRetryResult uploadDataAndRetry(const ApiClient& api, const char* endpoint, Stream& stream,
                                                   size_t streamLen, uint retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const ApiClient& api, const char* endpoint, File& f,
                                                        bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const ApiClient& api, const char* endpoint,
                                                        const char* filePath,
                                                        bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);

    bool disconnectNetwork() const;
    bool enableGPS();
    bool disableGPS();
    bool waitForRDY(uint32_t timeout_ms);

    // Funktion fragt der locale zeit von GSM Modem ab und gibt sie als String zurück
    // @result String - Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    [[nodiscard]] String getGSMDateTime(const TinyGSMDateTimeFormat format = DATE_FULL) const
    {
        return gsmModem.getGSMDateTime(format);
    }

    bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute, int* second, float* timezone) const
    {
        return gsmModem.getNetworkTime(year, month, day, hour, minute, second, timezone);
    }

    void getRevision(String& model, String& revision) const
    {
        gsmModem.getRevision(model, revision);
    }

    [[nodiscard]] time_t getUnixTimestamp() const;
    bool getGPS(GPS_DATA_t& out) const;

    [[nodiscard]] String getIMEI() const
    {
        return gsmModem.getIMEI();
    }

    [[nodiscard]] int16_t getSignalQuality() const
    {
        return gsmModem.getSignalQuality();
    }

    [[nodiscard]] bool isSleeping() const
    {
        return !modemIsAwake;
    }

    [[nodiscard]] bool isGPSEnabled() const
    {
        return gpsIsEnabled;
    }
};
