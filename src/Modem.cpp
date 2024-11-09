// Modem.cpp

#include "Modem.h"

#define SerialMon Serial
#define SerialAT Serial1

HttpClient *Modem::http = nullptr;

extern Config config;

Modem::Modem() : modem(SerialAT), client(modem) {}

bool Modem::init(bool secoundTry)
{
    SerialMon.println("Initializing modem...");

#ifdef TINY_GSM_T_PCIE
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
#endif

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

    if (config.GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(config.GSM_PIN);
    }

    modem.gprsConnect(config.apn, config.gprsUser, config.gprsPass);

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
    http->stop();
    modem.gprsDisconnect();
    SerialMon.println("Modem disconnected");
}

int Modem::sendRequest(String path, String method, String body)
{
    int err = 0;
    if (http == nullptr)
    {
        http = new HttpClient(client, config.server, config.port);
    }
    http->connectionKeepAlive();
    http->beginRequest();

    if (method == "GET")
    {
        err = http->get(path);
        http->sendBasicAuth(config.username, config.password);
    }
    else if (method == "POST")
    {
        err = http->post(path);
        http->sendBasicAuth(config.username, config.password);
        http->sendHeader("Content-Type", "application/json");
        http->sendHeader("Content-Length", body.length());
        http->beginBody();

        const size_t chunkSize = 512;
        size_t bodyLength = body.length();
        for (size_t i = 0; i < bodyLength; i += chunkSize)
        {
            String chunk = body.substring(i, min(i + chunkSize, bodyLength));
            http->print(chunk);
        }
    }
    http->endRequest();
    return err;
}

// Funktion fragt der locale zeit von GSM Modem ab und gibt sie als String zurück
// @result String - Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
String Modem::getLocalTime()
{
    String time = modem.getGSMDateTime(DATE_FULL);
    return time;
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

    int status = http->responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);
    if (!status)
    {
        SerialMon.println(F("no response"));
        delay(10000);
        http->stop();
        return nullptr;
    }

    if (status != 200)
    {
        SerialMon.println("unerwartete Antwort");
        http->stop();
        return nullptr;
    }

    SerialMon.println(F("Response Headers:"));
    while (http->headerAvailable())
    {
        String headerName = http->readHeaderName();
        String headerValue = http->readHeaderValue();
        SerialMon.println("    " + headerName + " : " + headerValue);
    }
    int contentLength = http->contentLength();
    String responseBody = "";
    if (contentLength > 0)
    {
        int readBytes = 0;
        char buffer[128];
        while (readBytes < contentLength)
        {
            int bytesToRead = http->readBytes(buffer, sizeof(buffer) - 1);
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
        rfidArray[i] = HelperUtils::toUpperCase(array[i]["rfid"].as<String>());
    }

    return rfidArray;
}

void printPercent(uint32_t readLength, uint32_t contentLength)
{
    if (contentLength != (uint32_t)-1)
    {
        SerialMon.print("\r ");
        SerialMon.print((100.0 * readLength) / contentLength);
        SerialMon.print('%');
    }
    else
    {
        SerialMon.println(readLength);
    }
}

void Modem::firmwareCheckAndUpdateIfNeeded()
{
    JsonDocument body;
    body["mac_address"] = MAC_ADDRESS;
    body["firmware_version"] = FIRMWARE_VERSION;
    serializeJsonPretty(body, SerialMon);

    SerialMon.println("");

    int err = sendRequest("/firmware/", "POST", body.as<String>());
    if (err != 0)
    {
        SerialMon.println(F("failed to connect 'firmware'"));
        SerialMon.println(err);
        return;
    }
    int status = http->responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);

    if (!status)
    {
        SerialMon.println(F("no response"));
        http->stop();
        return;
    }
    if (status == 200)
    {
        SerialMon.println("kein Update notwendig");
        http->stop();
        return;
    }
    if (status != 210)
    {
        SerialMon.println("unerwartete Antwort");
        http->stop();
        return;
    }
    else
    {
        SerialMon.println(F("Response Headers:"));
        while (http->headerAvailable())
        {
            String headerName = http->readHeaderName();
            String headerValue = http->readHeaderValue();
            SerialMon.println("    " + headerName + " : " + headerValue);
        }

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
        int contentLength = http->contentLength();
        const size_t bufferSize = 2048;
        uint8_t buffer[bufferSize];
        size_t totalBytesRead = 0;
        unsigned long startTime = millis();
        while (http->connected() || http->available())
        {
            if (http->available())
            {
                size_t bytesRead = http->readBytes(buffer, bufferSize);
                file.write(buffer, bytesRead);
                totalBytesRead += bytesRead;
                float percentage = ((float)totalBytesRead * 100) / contentLength;
                unsigned long elapsedTime = millis() - startTime;
                SerialMon.printf("\rProgress: %.2f%% Speed: %.2fKB/s Elapsed Time: %lu ms", percentage, (float)totalBytesRead / elapsedTime, elapsedTime);
            }
        }
        SerialMon.println("Total Bytes Read: " + String(totalBytesRead));
        SerialMon.printf("\rProgress: %.2f%%", (float)totalBytesRead / contentLength * 100);
        file.close();
        http->stop();
        SerialMon.println("File saved successfully");
        SPIFFSUtils::performOTAUpdateFromSPIFFS();
    }
}

// liefert immer true zurück
// rückgabe ist nur dafür da, damit der loop auf die funktion warten kann
bool Modem::uploadLogs()
{
    if (!SPIFFS.begin())
    {
        SerialMon.println("SPIFFS Mount Failed.");
        return true;
    }

    File file = SPIFFS.open(LOG_FILE_NAME, FILE_READ);
    if (!file)
    {
        SerialMon.println("File doesn't exist. Creating a new one.");
        return true;
    }

    int contentLength = file.size();
    if (contentLength == 0)
    {
        SerialMon.println("File is empty");
        return true;
    }
    SerialMon.print("File size: ");
    SerialMon.println(contentLength);

    SerialMon.println("Uploading logs to server...");
    int err = sendRequest("/logs/", "POST", file.readString());
    if (err != 0)
    {
        SerialMon.println(F("failed to connect 'logs'"));
        SerialMon.println(err);
        return true;
    }

    int status = http->responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);

    if (!status)
    {
        SerialMon.println(F("no response"));
        http->stop();
        return true;
    }

    if (status != 201)
    {
        SerialMon.println("unerwartete Antwort");
        http->stop();
        return true;
    }

    SerialMon.println(F("Response Headers:"));
    while (http->headerAvailable())
    {
        String headerName = http->readHeaderName();
        String headerValue = http->readHeaderValue();
        SerialMon.println("    " + headerName + " : " + headerValue);
    }

    SerialMon.println(F("Response Body:"));
    while (http->available())
    {
        SerialMon.write(http->read());
    }

    http->stop();
    file.close();
    SPIFFS.remove(LOG_FILE_NAME);
    SerialMon.println("Logs uploaded successfully");
    return true;
}