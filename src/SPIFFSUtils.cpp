// SPIFFSUtils.cpp
#include "SPIFFSUtils.h"
#define SerialMon Serial

void SPIFFSUtils::saveRfidsToSPIFFS(String *rfids, int arraySize)
{
    if (!SPIFFS.begin())
    {
        SerialMon.println("SPIFFS Mount Failed. Formatting...");
        if (!SPIFFS.format())
        {
            SerialMon.println("SPIFFS Formatting Failed");
            return;
        }
        if (!SPIFFS.begin())
        {
            SerialMon.println("SPIFFS Mount Failed After Formatting");
            return;
        }
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
        SerialMon.println("SPIFFS Mount Failed. Formatting...");
        if (!SPIFFS.format())
        {
            SerialMon.println("SPIFFS Formatting Failed");
            return false;
        }
        if (!SPIFFS.begin())
        {
            SerialMon.println("SPIFFS Mount Failed After Formatting");
            return false;
        }
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
