// Modem.h
#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>
#include <Config.h>
#include <TinyGsmClient.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <SPIFFSUtils.h>
#include <HelperUtils.h>

class Modem
{
public:
    Modem();
    bool init(bool secoundTry = false);
    void end();
    String getLocalTime();
    int sendRequest(String path, String method, String body = "");
    String* getRfids(int& arraySize);
    void firmwareCheckAndUpdateIfNeeded();
    void handleFirmwareUpdateWithWatchdog();
    bool uploadLogs();
    bool uploadGPSLog();
    bool uploadGPSLogAndDelete(bool deleteRegardlessOfSuccess);
    bool enableGPS();
    bool disableGPS();
    bool getGPSParsed(float* UTCDateAndTime = nullptr, float* latitude = nullptr, float* longitude = nullptr,
                      float* mslAltitude = nullptr, float* speedOverGround = nullptr, int retries = 1,
                      int retryDelayMs = 1000);
    bool getGPSRaw(String* rawGPSData, int retries = 1, int retryDelayMs = 1000);

private:
    TinyGsm modem;
    TinyGsmClient client;
    static HttpClient* http;
};

#endif
