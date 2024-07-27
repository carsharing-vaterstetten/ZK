// Modem.cpp
#include "Modem.h"

#define SerialMon Serial
#define SerialAT Serial1

Modem::Modem() : modem(SerialAT), client(modem), http(client, server, port) {}

String toUpperCase(const String &str)
{
    String upperStr = str;
    for (int i = 0; i < upperStr.length(); i++)
    {
        upperStr[i] = toupper(upperStr[i]);
    }
    return upperStr;
}

bool Modem::init(bool secoundTry)
{
    SerialMon.println("Initializing modem...");
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1000);
    digitalWrite(PWR_PIN, HIGH);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    sleep(1);
    modem.restart();
    sleep(1);

    String modemInfo = modem.getModemInfo();
    SerialMon.println("Modem Info: " + modemInfo);

    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }

    modem.gprsConnect(apn, gprsUser, gprsPass);

    SerialMon.print("Network status: ");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
    }

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
        return true;
    }
    else
    {
        if (!secoundTry)
        {
            return init(true);
        }
    }
    return false;
}

void Modem::end()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    http.stop();
    modem.gprsDisconnect();
    SerialMon.println("Modem disconnected");
}

int Modem::sendRequest(String path, String method, String body)
{
    int err = 0;
    http.connectionKeepAlive();
    http.beginRequest();

    if (method == "GET")
    {
        err = http.get(path);
    }
    else if (method == "POST")
    {
        err = http.post(path, "application/json", body);
    }
    http.sendBasicAuth(username, password);
    http.endRequest();
    return err;
}

String *Modem::getRfids(int &arraySize)
{
    SerialMon.print(F("Performing HTTPS GET request... "));
    int err = sendRequest("/rfids/", "GET");
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

    return rfidArray;
}

void Modem::firmwareCheckAndUpdateIfNeeded()
{
    byte mac[6];
    WiFi.macAddress(mac);
    String macStr = "";
    for (int i = 0; i < 6; i++)
    {
        if (mac[i] < 0x10)
        {
            macStr += '0';
        }
        macStr += String(mac[i], HEX);
        if (i < 5)
        {
            macStr += ':';
        }
    }
    macStr = toUpperCase(macStr);
    
    JsonDocument body;
    body["mac_address"] = macStr;
    body["firmware_version"] = FIRMWARE_VERSION;
    serializeJsonPretty(body, SerialMon);

    int err = sendRequest("/firmware/", "POST", body.as<String>());
    if (err != 0)
    {
        SerialMon.println(F("failed to connect 'firmware'"));
        SerialMon.println(err);
        return;
    }
    int status = http.responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);

    SerialMon.println(F("Response Headers:"));
    while (http.headerAvailable())
    {
        String headerName = http.readHeaderName();
        String headerValue = http.readHeaderValue();
        SerialMon.println("    " + headerName + " : " + headerValue);
    }

    if (!status)
    {
        return;
    }
    if (status != 210)
    {
        SerialMon.println(http.responseBody());
    }
    else
    {
        if (!SPIFFS.begin(true))
        {
            Serial.println("SPIFFS Mount Failed");
            return;
        }
        if (SPIFFS.exists(FIRMWARE_FILE_NAME))
        {
            SPIFFS.remove(FIRMWARE_FILE_NAME);
        }
        File file = SPIFFS.open(FIRMWARE_FILE_NAME, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return;
        }

        SerialMon.println("Getting Firmware data starting ...");
        int contentLength = http.contentLength();
        const size_t bufferSize = 2048;
        uint8_t buffer[bufferSize];
        size_t totalBytesRead = 0;
        while (http.connected() || http.available())
        {
            if (http.available())
            {
                size_t bytesRead = http.readBytes(buffer, bufferSize);
                file.write(buffer, bytesRead);
                totalBytesRead += bytesRead;
                float percentage = (float)totalBytesRead / contentLength * 100;
                SerialMon.printf("\rProgress: %.2f%%", percentage);
            }
        }
        SerialMon.println("Total Bytes Read: " + String(totalBytesRead));
        SerialMon.printf("\rProgress: %.2f%%", (float)totalBytesRead / contentLength * 100);
        file.close();
        SerialMon.println("File saved successfully");
        SPIFFSUtils::performOTAUpdateFromSPIFFS();
    }
}