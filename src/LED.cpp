#include "LED.h"

LED::LED(const uint16_t ledCount, const int16_t ledPin, const neoPixelType type) : neo{ledCount, ledPin, type} {}

bool LED::begin()
{
    return neo.begin();
}

void LED::setColor(const uint32_t hex)
{
    neo.fill(hex);
    neo.show();
}

void LED::clear()
{
    neo.clear();
    neo.show();
}

void LED::flash(const uint32_t hexColor, const uint16_t durationMs)
{
    setColor(hexColor);
    delay(durationMs);
    clear();
}

void StatusLED::setStatusColor(const StatusColor color)
{
    setColor(getStatusColorValue(color));
}

void StatusLED::flash(const StatusColor color, const uint16_t durationMs)
{
    setStatusColor(color);
    delay(durationMs);
    clear();
}

uint32_t StatusLED::getStatusColorValue(const StatusColor color)
{
    switch (color)
    {
    case StatusColor::PerformingOTAUpdate:
        return 0x800080;
    case StatusColor::Error:
        return 0xFF0000;
    case StatusColor::InitializationPhase:
        return 0xFFFFFF;
    case StatusColor::UpdatingRFIDs:
        return 0xFFA500;
    case StatusColor::UploadingLogs:
        return 0x0000FF;
    case StatusColor::CarLocked:
        return 0xFF0000;
    case StatusColor::CarUnlocked:
        return 0x00FF00;
    case StatusColor::NFCUnknownUIDScanned:
        return 0xFF0000;
    case StatusColor::WaitingForNFCCardToBeRemoved:
        return 0x00FFFF;
    default:
        return 0;
    }
}

CardReaderLED::CardReaderLED(const uint16_t ledCount, const int16_t ledPin, const neoPixelType type) : StatusLED(
    ledCount, ledPin, type) {}

void CardReaderLED::unlockFlash()
{
    flash(StatusColor::CarUnlocked, 100);
    delay(100);
    flash(StatusColor::CarUnlocked, 100);
}

void CardReaderLED::lockFlash()
{
    flash(StatusColor::CarLocked, 100);
    delay(100);
    flash(StatusColor::CarLocked, 100);
}

void CardReaderLED::cardDeclinedFlash()
{
    flash(StatusColor::NFCUnknownUIDScanned, 1500);
}

void CardReaderLED::loadingCircleStart()
{
    loadingCircleIndex = 0;
}

void CardReaderLED::loadingCircleNext(const StatusColor color)
{
    neo.clear();

    neo.setPixelColor(loadingCircleIndex, getStatusColorValue(color));
    neo.show();

    loadingCircleIndex = (loadingCircleIndex + 1) % neo.numPixels();
}

void CardReaderLED::loadingCircleStop()
{
    clear();
}

void CardReaderLED::progressIndicatorNext(const StatusColor color, const float progress)
{
    const uint16_t activePixels = progress * static_cast<float>(neo.numPixels());

    const uint32_t colorVal = getStatusColorValue(color);

    neo.clear();

    for (uint16_t n = 0; n < activePixels; ++n)
        neo.setPixelColor(n, colorVal);

    const float progressDiff = progress - static_cast<float>(activePixels) / static_cast<float>(neo.numPixels());
    const float brightness = progressDiff * static_cast<float>(neo.numPixels());

    const uint8_t r = static_cast<float>(colorVal >> 16 & 0xFF) * brightness;
    const uint8_t g = static_cast<float>(colorVal >> 8 & 0xFF) * brightness;
    const uint8_t b = static_cast<float>(colorVal & 0xFF) * brightness;
    neo.setPixelColor(activePixels, r, g, b);

    neo.show();
}

void CardReaderLED::progressIndicatorStop()
{
    clear();
}
