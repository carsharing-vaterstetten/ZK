#pragma once

#include <Arduino.h>
#include "Config.h" // Needed for TinyGsmClientSIM7000.h. Do not remove
#include <TinyGsmClientSIM7000.h>
#include <ArduinoHttpClient.h>
#include <FS.h>

class Modem
{

    static void powerOn();
    static void powerOff();
public:
    static TinyGsmSim7000::GsmClientSim7000* gsmClient;

    Modem() = delete;

    static bool init(bool secondTry = false);
    static void end();
    static int uploadFile(const String& endpoint, File& f, String* response, const String& urlParams = "",
                          int bufferSize = 512);
    static int simpleGet(const String& aUrlPath, String* responseBody);
    static bool downloadFile(const String& remotePath, File& f, int bufferSize = 512);
    static bool uploadLog();
    static void uploadLogAndDelete(uint32_t deleteAfterNRetries);

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

    static uint64_t getUTCTimestamp();

    static bool isInitialized()
    {
        return isInit;
    }

private:
    static bool isInit;
    static TinyGsmSim7000* gsmModem;
};
