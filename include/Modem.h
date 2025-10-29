#pragma once

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1KiB

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <FS.h>
#include "WatchdogHandler.h"
#include "GPS.h"

#define BASE_UPLOAD_RESULTS FILE_IS_EMPTY, UNEXPECTED_STATUS_CODE, HTTP_REQUEST_ERROR, FAILED_TO_INCREASE_TWDT_TIMEOUT, SUCCESS, FAILED_TO_SEND_DATA
#define BASE_DOWNLOAD_RESULTS FAILED_TO_OPEN_STREAM, SUCCESS, UNEXPECTED_STATUS_CODE

enum class UploadResult
{
    BASE_UPLOAD_RESULTS,
};

enum class UploadAndRetryResult
{
    BASE_UPLOAD_RESULTS,
    FILE_DOES_NOT_EXIST,
    FAILED_TO_OPEN_FILE,
    SUCCESS_AFTER_RETRYING,
};

enum class DownloadResult
{
    BASE_DOWNLOAD_RESULTS,
};

// HTTP download stream that automatically manages connection and watchdog timeout
class DownloadStream : public HttpClient
{
public:
    // Open HTTP GET stream with automatic setup and watchdog management
    DownloadStream(const String& remotePath, Client& gsmClient, const String& server, uint16_t port,
                   const String& username = "", const String& password = "", const String& cacheChecksum = "");

    // Constructor for invalid/null stream
    DownloadStream() : HttpClient(*(Client*)nullptr, "", 0), isValid(false), responseCode(0)
    {
    }

    ~DownloadStream() override
    {
        if (isValid)
        {
            HttpClient::stop();
            if (responseCode != 304)
                WatchdogHandler::revertTemporaryIncrease();
        }
    }

    explicit operator bool() const { return isValid; }

    [[nodiscard]] int responseStatusCode() const
    {
        return responseCode;
    }

private:
    bool isValid;
    int responseCode;
};

class Modem
{
    static void powerOn();
    static void powerOff();
    static bool enableGPS();
    static bool disableGPS();

    static uint32_t estimatedDownloadSpeed;
    static uint32_t estimatedUploadSpeed;

    static bool isInit, timeSynced;
    static TinyGsmSim7000* gsmModem;

public:
    static TinyGsmSim7000::GsmClientSim7000* gsmClient;

    Modem() = delete;

    static bool init(uint8_t retries = 2);
    static UploadResult uploadFile(const String& endpoint, File& f, int* statusCode, String* response,
                                   int bufferSize = 256, unsigned long* uploadStartMs = nullptr,
                                   unsigned long* uploadEndMs = nullptr);
    static UploadAndRetryResult uploadFileAndDelete(const String& endpoint, const String& filePath,
                                                    bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries,
                                                    int bufferSize = 256, unsigned long* uploadStartMs = nullptr,
                                                    unsigned long* uploadEndMs = nullptr);
    static int simpleGet(const String& aUrlPath, String* responseBody, const String& username = "",
                         const String& password = "");
    static int simpleGetBin(const String& aUrlPath, uint8_t* responseBody, size_t size, const String& username = "",
                            const String& password = "");
    static DownloadResult downloadFile(const String& remotePath, File& f, const String& username = "",
                                       const String& password = "", int bufferSize = 512,
                                       unsigned long* downloadStartMs = nullptr,
                                       unsigned long* downloadEndMs = nullptr);
    static void uploadLog(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);

    // Funktion fragt der locale zeit von GSM Modem ab und gibt sie als String zurück
    // @result String - Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    static String getGSMDateTime(const TinyGSMDateTimeFormat format = DATE_FULL)
    {
        return gsmModem->getGSMDateTime(format);
    }

    static bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute, int* second, float* timezone)
    {
        return gsmModem->getNetworkTime(year, month, day, hour, minute, second, timezone);
    }

    static time_t getUnixTimestamp();
    static esp_err_t increaseWatchdogTimeoutForFileUpload(size_t fileSize);
    static esp_err_t increaseWatchdogTimeoutForFileDownload(size_t fileSize);
    static void performConnectionSpeedTest();
    static bool getGPS(GPS_DATA_t& out);

    static bool isInitialized()
    {
        return isInit;
    }

    static bool timeIsAvailable()
    {
        return timeSynced;
    }

    static String getIMEI()
    {
        return gsmModem->getIMEI();
    }

    static uint32_t getEstimatedDownloadSpeed()
    {
        return estimatedDownloadSpeed;
    }

    static uint32_t getEstimatedUploadSpeed()
    {
        return estimatedUploadSpeed;
    }
};
