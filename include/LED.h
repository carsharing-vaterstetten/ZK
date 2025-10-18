#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum class StatusColor
{
    InitializationPhase,
    PerformingOTAUpdate,
    UpdatingRFIDs,
    UploadingLogs,
    Error,
    CarUnlocked,
    CarLocked,
    NFCUnknownUIDScanned,
};

class LED
{
public:
    LED(uint16_t ledCount, int16_t ledPin, neoPixelType type);

    bool init();
    void setStatusColor(StatusColor color);
    void clear();

private:
    Adafruit_NeoPixel neo;
    static uint32_t getStatusColorValue(StatusColor color);
};
