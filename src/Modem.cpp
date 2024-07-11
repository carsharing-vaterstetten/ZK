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
    modem.init();

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

    String body = http.responseBody();
    SerialMon.println(F("Response:"));
    SerialMon.println(body);

    SerialMon.print(F("Body length is: "));
    SerialMon.println(body.length());

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);
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