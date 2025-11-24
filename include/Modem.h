#pragma once

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1KiB

#include <TinyGsmClient.h>
#include <FS.h>

#include "ApiStreams.h"
#include "Config.h"
#include "GPS.h"

#define SERIAL_AT Serial1

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

class Modem
{
    static void powerOn();
    static void powerOff();
    bool enableGPS();
    bool disableGPS();

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
    bool ensureNetworkConnection();
    static ApiResponse uploadData(const char* endpoint, Stream& stream, uint32_t streamLen);
    static UploadAndRetryResult uploadDataAndRetry(const char* endpoint, Stream& stream, uint32_t streamLen,
                                                   uint32_t retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const char* endpoint, File& f, bool deleteIfSuccess,
                                                        bool deleteAfterRetrying, uint32_t retries);
    static UploadFileAndRetryResult uploadFileAndDelete(const char* endpoint, const char* filePath, bool deleteIfSuccess,
                                                        bool deleteAfterRetrying, uint32_t retries);
    static void uploadLog(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);

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
#if GIVE_CONNECTION_SPEED_ESTIMATE
    static void performConnectionSpeedTest();
#endif
    bool getGPS(GPS_DATA_t& out);

    [[nodiscard]] bool timeIsAvailable() const
    {
        return timeSynced;
    }

    String getIMEI()
    {
        return gsmModem.getIMEI();
    }

    int16_t getSignalQuality()
    {
        return gsmModem.getSignalQuality();
    }
};

inline Modem modem{115200, PIN_RX, PIN_TX};
