#include "LED.h"

LED::LED(Adafruit_NeoPixel& ledDriver) : neo(ledDriver) {}

void LED::setColor(const uint32_t hex) const
{
    neo.fill(hex);
    neo.show();
}

void LED::clear() const
{
    neo.clear();
    neo.show();
}

void StatusLED::setStatusColor(const StatusColor color) const
{
    setColor(getStatusColorValue(color));
}

void StatusLED::flash(const StatusColor color, const uint16_t durationMs) const
{
    setStatusColor(color);
    delay(durationMs);
    clear();
}

uint32_t StatusLED::getStatusColorValue(const StatusColor color)
{
    switch (color)
    {
    case StatusColor::PerformingOTAUpdate: return 0x800080;
    case StatusColor::Error: return 0xFF0000;
    case StatusColor::InitializationPhase: return 0xFFFFFF;
    case StatusColor::UpdatingRFIDs: return 0xFFA500;
    case StatusColor::UploadingLogs: return 0x0000FF;
    case StatusColor::CarLocked: return 0xFF0000;
    case StatusColor::CarUnlocked: return 0x00FF00;
    case StatusColor::NFCUnknownUIDScanned: return 0xFF0000;
    case StatusColor::WaitingForNFCCardToBeRemoved: return 0x00FFFF;
    default: return 0;
    }
}

void CardReaderLED::unlockFlash() const
{
    unlockFlashSequence.play();
}

void CardReaderLED::lockFlash() const
{
    flash(StatusColor::CarLocked, 100);
    delay(100);
    flash(StatusColor::CarLocked, 100);
}

void CardReaderLED::cardDeclinedFlash() const
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

void CardReaderLED::loadingCircleStop() const
{
    clear();
}

void CardReaderLED::progressIndicatorNext(const StatusColor color, const float progress) const
{
    const uint16_t totalPixels = neo.numPixels();
    const auto activePixels = static_cast<uint16_t>(progress * static_cast<float>(totalPixels));

    const uint32_t colorVal = getStatusColorValue(color);

    neo.clear();

    for (uint16_t n = 0; n < activePixels; ++n)
        neo.setPixelColor(n, colorVal);

    // Only paint the partial pixel if there's actually a pixel slot for it
    if (activePixels < totalPixels)
    {
        const float progressDiff = progress - static_cast<float>(activePixels) / static_cast<float>(totalPixels);
        const float brightness = progressDiff * static_cast<float>(totalPixels);

        const uint8_t r = static_cast<float>(colorVal >> 16 & 0xFF) * brightness;
        const uint8_t g = static_cast<float>(colorVal >> 8 & 0xFF) * brightness;
        const uint8_t b = static_cast<float>(colorVal & 0xFF) * brightness;
        neo.setPixelColor(activePixels, r, g, b);
    }

    neo.show();
}

void CardReaderLED::progressIndicatorStop() const
{
    clear();
}

void CardReaderLED::stopAllAnimations() const
{
    loadingCircleStop();
    progressIndicatorStop();
}
