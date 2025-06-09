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
    String *getRfids(int &arraySize);
    void firmwareCheckAndUpdateIfNeeded();
    void handleFirmwareUpdateWithWatchdog();
    bool uploadLogs();

private:
    TinyGsm modem;
    TinyGsmClient client;
    static HttpClient *http;
};

#endif
