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

    File file = SPIFFS.open("/rfids.txt", FILE_WRITE);
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

    File file = SPIFFS.open("/rfids.txt", FILE_READ);
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
