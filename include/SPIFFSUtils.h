// SPIFFSUtils.h
#ifndef SPIFFSUTILS_H
#define SPIFFSUTILS_H

#include <Arduino.h>
#include "SPIFFS.h"

class SPIFFSUtils
{
public:
    static void saveRfidsToSPIFFS(String *rfids, int arraySize);
    static bool isRfidInSPIFFS(String rfid);
};

#endif
