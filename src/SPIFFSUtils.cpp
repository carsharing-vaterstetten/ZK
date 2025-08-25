// SPIFFSUtils.cpp
#include "SPIFFSUtils.h"
#define SerialMon Serial
extern Modem modem;

void SPIFFSUtils::saveRfidsToSPIFFS(String* rfids, int arraySize)
{
    if (!SPIFFS.begin())
    {
        SerialMon.println("SPIFFS Mount Failed.");
        return;
    }

    File file = SPIFFS.open(RFID_FILE_NAME, FILE_WRITE);
    if (!file)
    {
        SerialMon.println("Failed to open file for writing");
        return;
    }

    file.println(arraySize);

    for (int i = 0; i < arraySize; i++)
    {
        file.println(rfids[i]);
    }
    file.close();
}

bool SPIFFSUtils::isRfidInSPIFFS(String rfid)
{
    if (!SPIFFS.begin())
    {
        SerialMon.println("SPIFFS Mount Failed.");
        return false;
    }

    File file = SPIFFS.open(RFID_FILE_NAME, FILE_READ);
    if (!file)
    {
        SerialMon.println("Failed to open file for reading");
        return false;
    }

    int arraySize = file.parseInt();
    file.readStringUntil('\n');

    for (int i = 0; i < arraySize; i++)
    {
        String storedRfid = file.readStringUntil('\n');
        storedRfid.trim();
        if (storedRfid == rfid)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void SPIFFSUtils::performOTAUpdateFromSPIFFS()
{
    // Open the firmware file in SPIFFS for reading
    Serial.println("Starting OTA update from SPIFFS");
    File file = SPIFFS.open(FIRMWARE_FILE_NAME, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Starting update..");
    size_t fileSize = file.size();
    Serial.println(fileSize);

    // Begin OTA update process with specified size and flash destination
    if (!Update.begin(fileSize, U_FLASH))
    {
        Serial.println("Cannot do the update");
        return;
    }

    // Write firmware data from file to OTA update
    Update.writeStream(file);

    // Complete the OTA update process
    if (Update.end())
    {
        Serial.println("Successful update");
    }
    else
    {
        Serial.println("Error Occurred:" + String(Update.getError()));
        return;
    }

    file.close(); // Close the file
    Serial.println("Reset in 4 seconds....");
    delay(4000);
    ESP.restart(); // Restart ESP32 to apply the update
}

void SPIFFSUtils::addLogEntry(const String& logText)
{
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount Failed.");
        return;
    }

    JsonDocument doc;

    File file = SPIFFS.open(LOG_FILE_NAME, FILE_READ);
    if (!file)
    {
        Serial.println("File doesn't exist. Creating a new one.");
        // If the file doesn't exist, create a new JSON structure
        doc["mac_address"] = MAC_ADDRESS;
        JsonArray logs = doc["logs"].to<JsonArray>();
    }
    else
    {
        // If the file exists, parse it
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.print(F("Failed to read file, using empty JSON. Error: "));
            Serial.println(error.c_str());
            doc["mac_address"] = MAC_ADDRESS;
            JsonArray logs = doc["logs"].to<JsonArray>();
        }
        file.close();
    }
    JsonArray logs = doc["logs"];

    JsonObject logs_o = logs.add<JsonObject>();
    logs_o["created_at"] = modem.getLocalTime();
    logs_o["firmware_version"] = FIRMWARE_VERSION;
    logs_o["log"] = logText;

    file = SPIFFS.open(LOG_FILE_NAME, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    file.close();
}

/// Adds a GPS entry to the GPS log file in SPIFFS. Returns true if successful, false otherwise.
bool SPIFFSUtils::addGPSEntry(const String& gpsText, const String& currentlyUsedRfid)
{
    Serial.print("Adding GPS entry to log... ");
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount Failed.");
        return false;
    }

    File file = SPIFFS.open(GPS_LOG_FILE_NAME, FILE_APPEND);

    if (!file)
    {
        Serial.println("Failed to open GPS log file for appending");
        return false;
    }

    file.print(gpsText);
    file.print(",");
    file.println(currentlyUsedRfid);
    file.close();

    Serial.println("Success");

    return true;
}
