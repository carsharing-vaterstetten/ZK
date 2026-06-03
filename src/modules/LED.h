#pragma once

#include <Adafruit_NeoPixel.h>

#include "abstract/SequencePlayer.h"

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

private:
    uint16_t loadingCircleIndex = 0;

    const SequencePlayer unlockFlashSequence{
        {
            SequencePoint{0, [this] { flash(StatusColor::CarUnlocked, 100); }},
            SequencePoint{200, [this] { flash(StatusColor::CarUnlocked, 100); }},
        }
    };
};
