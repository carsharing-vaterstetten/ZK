// Modem.h
#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>
#include <Config.h>
#include <TinyGsmClient.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

class Modem
{
public:
    Modem();
    void powerOn();
    void powerOff();
    void restart();
    String *getRfids(int &arraySize);

private:
    TinyGsm modem;
    TinyGsmClientSecure client;
    HttpClient http;
};

#endif
