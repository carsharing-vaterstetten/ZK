#pragma once

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // 1KiB

#include <TinyGsmClientSIM7000.h>
#include <ArduinoHttpClient.h>
#include <FS.h>

#define BASE_UPLOAD_RESULTS FILE_IS_EMPTY, HTTP_REQUEST_ERROR, FAILED_TO_INCREASE_TWDT_TIMEOUT, SUCCESS
#define BASE_DOWNLOAD_RESULTS HTTP_REQUEST_ERROR, UNEXPECTED_STATUS_CODE, FAILED_TO_INCREASE_TWDT_TIMEOUT, SUCCESS
#define UPLOAD_WITH_SIZE_CHECK_RESULTS BASE_UPLOAD_RESULTS, UNEXPECTED_STATUS_CODE, SIZE_CHECK_FAILED

enum class UploadResult
{
    BASE_UPLOAD_RESULTS,
};

enum class UploadWithSizeCheckResult
{
    UPLOAD_WITH_SIZE_CHECK_RESULTS
};

enum class UploadWithSizeCheckResultAndRetries
{
    UPLOAD_WITH_SIZE_CHECK_RESULTS,
    FILE_DOES_NOT_EXIST,
    FAILED_TO_OPEN_FILE,
    SUCCESS_AFTER_RETRYING,
};

enum class DownloadResult
{
    BASE_DOWNLOAD_RESULTS,
};

#pragma pack(push, 1)
struct GPS_DATA_t
{
    float lat = 0.0;
    float lon = 0.0;
    float speed = 0;
    float alt = 0;
    uint8_t vsat = 0;
    uint8_t usat = 0;
    float accuracy = 0;
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
};
#pragma pack(pop)

class Modem
{
    static void powerOn();
    static void powerOff();
    static bool enableGPS();
    static bool disableGPS();

    static uint32_t estimatedDownloadSpeed;
    static uint32_t estimatedUploadSpeed;

public:
    static TinyGsmSim7000::GsmClientSim7000* gsmClient;

    Modem() = delete;

    static bool init(uint8_t retries = 2);
    static UploadResult uploadFile(const String& endpoint, File& f, int* statusCode, String* response,
                                   const String& urlParams = "", int bufferSize = 512,
                                   unsigned long* uploadStartMs = nullptr, unsigned long* uploadEndMs = nullptr);
    static UploadWithSizeCheckResult uploadFileWithSizeCheck(const String& endpoint, File& f,
                                                             const String& urlParams = "",
                                                             int bufferSize = 512,
                                                             unsigned long* uploadStartMs = nullptr,
                                                             unsigned long* uploadEndMs = nullptr);
    static UploadWithSizeCheckResultAndRetries uploadFileWithSizeCheckAndDelete(const String& endpoint, FS& fileFs, const String& filePath,
                                                 bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries,
                                                 const String& urlParams = "", int bufferSize = 512,
                                                 unsigned long* uploadStartMs = nullptr,
                                                 unsigned long* uploadEndMs = nullptr);
    static int simpleGet(const String& aUrlPath, String* responseBody, const String& username = "",
                         const String& password = "");
    static DownloadResult downloadFile(const String& remotePath, File& f, const String& username = "",
                                       const String& password = "", int bufferSize = 512,
                                       unsigned long* downloadStartMs = nullptr,
                                       unsigned long* downloadEndMs = nullptr);
    static void uploadFileFromAllFileSystem(const String& filePath, const String& endpoint, bool deleteIfSuccess,
                                            bool deleteAfterRetrying, uint32_t retries);
    static void uploadLogsFromAllFileSystems(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);

    // Funktion fragt der locale zeit von GSM Modem ab und gibt sie als String zurück
    // @result String - Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    static String getLocalTime(const TinyGSMDateTimeFormat format = DATE_FULL)
    {
        return gsmModem->getGSMDateTime(format);
    }

    static bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute, int* second, float* timezone)
    {
        return gsmModem->getNetworkTime(year, month, day, hour, minute, second, timezone);
    }

    static uint64_t getUnixTimestamp();
    static esp_err_t increaseWatchdogTimeoutForFileUpload(size_t fileSize);
    static esp_err_t increaseWatchdogTimeoutForFileDownload(size_t fileSize);
    static void performConnectionSpeedTest();
    static bool getGPS(GPS_DATA_t& out);
    static void uploadGPSFile(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);

    static bool isInitialized()
    {
        return isInit;
    }

private:
    static bool isInit;
    static TinyGsmSim7000* gsmModem;
};
