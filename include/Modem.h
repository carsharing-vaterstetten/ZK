#pragma once

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1KiB

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <FS.h>
#include "Config.h"
#include "WatchdogHandler.h"
#include "GPS.h"

#define SERIAL_AT Serial1

#define BASE_UPLOAD_RESULTS UNEXPECTED_STATUS_CODE, HTTP_REQUEST_ERROR, FAILED_TO_INCREASE_TWDT_TIMEOUT, SUCCESS, FAILED_TO_SEND_DATA
#define BASE_DOWNLOAD_RESULTS FAILED_TO_OPEN_STREAM, SUCCESS, UNEXPECTED_STATUS_CODE

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

enum class DownloadResult
{
    BASE_DOWNLOAD_RESULTS,
};

class RandomStream final : public Stream
{
    int available() override
    {
        return 1; // always something to read
    }

    int read() override
    {
        return random(0, 256); // return random byte
    }

    int peek() override
    {
        return random(0, 256); // not really peek-safe, but fine for quick & dirty
    }

    void flush() override
    {
        // nothing to flush
    }

    size_t write(uint8_t) override
    {
        return 1; // discard writes
    }
};

// HTTP download stream that automatically manages connection and watchdog timeout
class DownloadStream : public HttpClient
{
public:
    // Open HTTP GET stream with automatic setup and watchdog management
    DownloadStream(const String& remotePath, Client& gsmClient, const String& server, uint16_t port,
                   const String& username = "", const String& password = "", const String& cacheChecksum = "");

    explicit DownloadStream(const String& remotePath, const String& cacheChecksum = "");

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
                watchdogHandler.revertTemporaryIncrease();
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
    bool enableGPS();
    bool disableGPS();

    uint32_t estimatedDownloadSpeed = 3000;
    uint32_t estimatedUploadSpeed = 1000;
    bool timeSynced = false;

    unsigned long serialBaud;
    int8_t rxPin, txPin;
    TinyGsmSim7000 gsmModem{SERIAL_AT};

public:
    TinyGsmSim7000::GsmClientSim7000 gsmClient{gsmModem}; // TODO: abstract this

    Modem(const uint32_t serialBaud, const int8_t rxPin,
          const int8_t txPin) : serialBaud(serialBaud), rxPin(rxPin), txPin(txPin)
    {
    }

    bool init(uint8_t retries = 2);
    UploadResult uploadData(const String& endpoint, Stream& stream, uint32_t streamLen, int* statusCode,
                            String* response,
                            unsigned long* uploadStartMs = nullptr, unsigned long* uploadEndMs = nullptr,
                            bool logToLogFile = true);
    UploadAndRetryResult uploadDataAndRetry(const String& endpoint, Stream& stream, uint32_t streamLen,
                                            uint32_t retries, unsigned long* uploadStartMs, unsigned long* uploadEndMs,
                                            bool logToLogFile);
    UploadFileAndRetryResult uploadFileAndDelete(const String& endpoint, File& f, bool deleteIfSuccess,
                                                 bool deleteAfterRetrying, uint32_t retries);
    UploadFileAndRetryResult uploadFileAndDelete(const String& endpoint, const String& filePath, bool deleteIfSuccess,
                                                 bool deleteAfterRetrying, uint32_t retries);
    int simpleGet(const String& aUrlPath, String* responseBody, const String& username = "",
                  const String& password = "");
    int simpleGetBin(const String& aUrlPath, uint8_t* responseBody, size_t size, const String& username = "",
                     const String& password = "");
    DownloadResult downloadData(const String& remotePath, Stream& f, const String& username = "",
                                const String& password = "",
                                unsigned long* downloadStartMs = nullptr,
                                unsigned long* downloadEndMs = nullptr);
    void uploadLog(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);

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
    esp_err_t increaseWatchdogTimeoutForFileUpload(size_t fileSize) const;
    esp_err_t increaseWatchdogTimeoutForFileDownload(size_t fileSize) const;
    void performConnectionSpeedTest();
    bool getGPS(GPS_DATA_t& out);

    [[nodiscard]] bool timeIsAvailable() const
    {
        return timeSynced;
    }

    String getIMEI()
    {
        return gsmModem.getIMEI();
    }

    [[nodiscard]] uint32_t getEstimatedDownloadSpeed() const
    {
        return estimatedDownloadSpeed;
    }

    [[nodiscard]] uint32_t getEstimatedUploadSpeed() const
    {
        return estimatedUploadSpeed;
    }

    int16_t getSignalQuality()
    {
        return gsmModem.getSignalQuality();
    }
};

inline Modem modem{115200, PIN_RX, PIN_TX};
