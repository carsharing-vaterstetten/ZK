#pragma once

#include <FS.h>
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB
#include <TinyGsmClient.h>

#include "ApiStreams.h"
#include "GPS.h"

#define BASE_UPLOAD_RESULTS SUCCESS, FAILED

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

class Modem
{
protected:
    bool modemIsAwake = false, gpsIsEnabled = false;
    ulong serialBaud;
    int8_t rxPin, txPin;

    TinyGsmSim7000 gsmModem;
    const char *gprsUser = "", *gprsPassword = "", *apn = "";
    HardwareSerial& serial;

    bool beginSleep();
    std::tuple<bool, ulong> autoBaud(uint32_t timeoutMs);
    bool connectNetwork(uint retries);
    bool connectGPRS(uint retries);
    bool beginHot(const char* simPin);
    bool beginCold(const char* simPin, uint retries);
    bool finishInit(const char* simPin, ulong detectedBaud);
    static void forcePowerCycle();

public:
    TinyGsmSim7000::GsmClientSim7000 gsmClient;

    Modem(HardwareSerial& hwSerial, ulong serialBaud, int8_t rxPin, int8_t txPin);

    bool powerOff();
    static void powerOn();
    static void turnOn();
    static void turnOff();
    SleepRequestResult requestSleep();

    bool begin(const char* simPin, const char* user, const char* password, const char* netApn, uint retries = 2);
    bool ensureNetworkConnection(uint maxRetries = 2, bool connectNetworkFirst = true);
    void wakeup();
    void wakeupAndWait(uint32_t timeoutMs = 10000);
    static ApiResponse uploadData(const char* endpoint, Stream& stream, size_t streamLen);
    static UploadAndRetryResult uploadDataAndRetry(const char* endpoint, Stream& stream, size_t streamLen,
                                                   uint retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const char* endpoint, File& f, bool deleteIfSuccess,
                                                        bool deleteAfterRetrying, uint retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const char* endpoint, const char* filePath,
                                                        bool deleteIfSuccess,
                                                        bool deleteAfterRetrying, uint retries);

    bool disconnectNetwork();
    bool enableGPS();
    bool disableGPS();

    // Funktion fragt der locale zeit von GSM Modem ab und gibt sie als String zurück
    // @result String - Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    String getGSMDateTime(const TinyGSMDateTimeFormat format = DATE_FULL)
    {
        return gsmModem.getGSMDateTime(format);
    }

    bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute, int* second, float* timezone)
    {
        return gsmModem.getNetworkTime(year, month, day, hour, minute, second, timezone);
    }

    [[nodiscard]] time_t getUnixTimestamp();
    bool getGPS(GPS_DATA_t& out);

    String getIMEI()
    {
        return gsmModem.getIMEI();
    }

    int16_t getSignalQuality()
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
