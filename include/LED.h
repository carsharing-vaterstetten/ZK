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
    void flash(StatusColor color, uint16_t durationMs);

private:
    Adafruit_NeoPixel neo;
    static uint32_t getStatusColorValue(StatusColor color);
};

class CardReaderLED : public LED
{
public:
    CardReaderLED(const uint16_t ledCount, const int16_t ledPin, const neoPixelType type)
        : LED(ledCount, ledPin, type)
    {
    }

    void unlockFlash();
    void lockFlash();
    void cardDeclinedFlash();
};
