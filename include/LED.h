#pragma once

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
    WaitingForNFCCardToBeRemoved,
};

class LED
{
public:
    LED(uint16_t ledCount, int16_t ledPin, neoPixelType type);

    bool begin();
    void setColor(uint32_t hex);
    void clear();
    void flash(uint32_t hexColor, uint16_t durationMs);

protected:
    Adafruit_NeoPixel neo;
};

class StatusLED : public LED
{
public:
    using LED::LED;
    void setStatusColor(StatusColor color);
    void flash(StatusColor color, uint16_t durationMs);
    static uint32_t getStatusColorValue(StatusColor color);
};

class CardReaderLED : public StatusLED
{
public:
    CardReaderLED(uint16_t ledCount, int16_t ledPin, neoPixelType type);

    void unlockFlash();
    void lockFlash();
    void cardDeclinedFlash();
};
