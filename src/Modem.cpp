// Modem.cpp
#include "Modem.h"

#define SerialMon Serial
#define SerialAT Serial1

Modem::Modem() : modem(SerialAT), client(modem), http(client, server, port) {}

void Modem::powerOn()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1000);
    digitalWrite(PWR_PIN, HIGH);
}

void Modem::powerOff()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1500);
    digitalWrite(PWR_PIN, HIGH);
}

void Modem::restart()
{
    powerOff();
    delay(1000);
    powerOn();
}

String toUpperCase(const String &str)
{
    String upperStr = str;
    for (int i = 0; i < upperStr.length(); i++)
    {
        upperStr[i] = toupper(upperStr[i]);
    }
    return upperStr;
}

String *Modem::getRfids(int &arraySize)
{
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    SerialMon.println("Initializing modem...");
    sleep(1);
    modem.init();
    sleep(1);

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }

    modem.gprsConnect(apn, gprsUser, gprsPass);
    SerialMon.println("GPRS connected");

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        return nullptr;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
    }

    SerialMon.print(F("Performing HTTPS GET request... "));
    http.connectionKeepAlive();
    http.beginRequest();
    int err = http.get("/rfids/");
    http.sendBasicAuth(username, password);
    http.endRequest();
    if (err != 0)
    {
        SerialMon.println(F("failed to connect"));
        SerialMon.println(err);
        delay(10000);
        return nullptr;
    }

    int status = http.responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);
    if (!status)
    {
        delay(10000);
        return nullptr;
    }

    SerialMon.println(F("Response Headers:"));
    while (http.headerAvailable())
    {
        String headerName = http.readHeaderName();
        String headerValue = http.readHeaderValue();
        SerialMon.println("    " + headerName + " : " + headerValue);
    }
    int contentLength = http.contentLength();
    String responseBody = "";
    if (contentLength > 0)
    {
        int readBytes = 0;
        char buffer[128];
        while (readBytes < contentLength)
        {
            int bytesToRead = http.readBytes(buffer, sizeof(buffer) - 1);
            readBytes += bytesToRead;
            buffer[bytesToRead] = '\0';
            responseBody += buffer;
        }
    }

    SerialMon.println(F("Response Body:"));
    SerialMon.println(responseBody);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, responseBody);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return nullptr;
    }
    JsonArray array = doc.as<JsonArray>();

    arraySize = array.size();
    String *rfidArray = new String[arraySize];

    for (int i = 0; i < arraySize; i++)
    {
        rfidArray[i] = toUpperCase(array[i]["rfid"].as<String>());
    }

    http.stop();
    SerialMon.println(F("Server disconnected"));

    modem.gprsDisconnect();
    SerialMon.println(F("GPRS disconnected"));

    return rfidArray;
}