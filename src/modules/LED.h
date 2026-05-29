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
    explicit LED(Adafruit_NeoPixel& ledDriver);

    void setColor(uint32_t hex) const;
    void clear() const;
    void flash(uint32_t hexColor, uint16_t durationMs) const;

protected:
    Adafruit_NeoPixel& neo;
};

class StatusLED : public LED
{
public:
    using LED::LED;
    void setStatusColor(StatusColor color) const;
    void flash(StatusColor color, uint16_t durationMs) const;
    static uint32_t getStatusColorValue(StatusColor color);
};

class CardReaderLED : public StatusLED
{
public:
    using StatusLED::StatusLED;

    void unlockFlash() const;
    void lockFlash() const;
    void cardDeclinedFlash() const;

    void loadingCircleStart();
    void loadingCircleNext(StatusColor color);
    void loadingCircleStop() const;

    void progressIndicatorNext(StatusColor color, float progress) const;
    void progressIndicatorStop() const;

protected:
    uint16_t loadingCircleIndex = 0;
};
