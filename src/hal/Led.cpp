#include "hal/Led.h"

Led::Led(Adafruit_NeoPixel& ledDriver) : neo(ledDriver) {}

void Led::setColor(const uint32_t hex) const
{
    neo.fill(hex);
    neo.show();
}

void Led::clear() const
{
    neo.clear();
    neo.show();
}

void Led::renderState(const ProgressState& state) const
{
    switch (state.effect)
    {
    case LedEffect::Bar:
        renderProgressBar(state.progress, state.colorHex);
        break;
    case LedEffect::Pulse:
        renderPulse(state.colorHex);
        break;
    case LedEffect::Fade:
        renderFade(state.progress, state.colorHex);
        break;
    }
}

void Led::renderProgressBar(const float progress, const uint32_t colorHex) const
{
    const uint16_t pixelCount = neo.numPixels();
    const float filled = std::clamp(progress, 0.0f, 1.0f) * static_cast<float>(pixelCount);

    neo.setBrightness(255); // dimming happens in the colour, not globally

    for (uint16_t i = 0; i < pixelCount; ++i)
    {
        // How much of this pixel the bar covers: 1 fully, 0 not at all.
        const float coverage = std::clamp(filled - static_cast<float>(i), 0.0f, 1.0f);
        neo.setPixelColor(i, scaleColorBrightness(colorHex, coverage));
    }

    neo.show();
}

void Led::renderFade(const float level, const uint32_t colorHex) const
{
    neo.setBrightness(255);
    neo.fill(scaleColorBrightness(colorHex, perceptual(level)));
    neo.show();
}

void Led::renderPulse(const uint32_t colorHex) const
{
    constexpr uint32_t periodMs = 1400;
    constexpr float floorBrightness = 0.15f;

    // Raised cosine: symmetric and never dark, which is what keeps this distinct
    // from the spinner's rise-fade-gap even when the two share a colour.
    const float wave = 0.5f * (1.0f - cosf(2.0f * static_cast<float>(PI) * animationPhase(periodMs)));

    renderFade(floorBrightness + (1.0f - floorBrightness) * wave, colorHex);
}

void Led::renderSpinner(const uint32_t colorHex) const
{
    const uint16_t pixelCount = neo.numPixels();

    neo.setBrightness(255);

    if (pixelCount <= 1)
    {
        neo.setPixelColor(0, scaleColorBrightness(
                              colorHex, passingCometBrightness(animationPhase(singlePixelRevolutionMs))));
        neo.show();
        return;
    }

    // Fractional, so the head glides between LEDs instead of stepping.
    const float head = animationPhase(pixelCount * spinnerMsPerPixel) * pixelCount;

    for (uint16_t i = 0; i < pixelCount; ++i)
    {
        float behind = head - static_cast<float>(i);
        if (behind < 0.0f) behind += static_cast<float>(pixelCount);

        neo.setPixelColor(i, scaleColorBrightness(colorHex, tailBrightness(behind)));
    }

    neo.show();
}

float Led::tailBrightness(const float behind)
{
    if (behind >= spinnerTailPixels) return 0.0f;

    const float t = 1.0f - behind / spinnerTailPixels;
    return t * t; // squared so the head stays crisp and the trail falls away
}

float Led::passingCometBrightness(const float phase)
{
    constexpr float riseEnd = 0.12f;
    constexpr float fadeEnd = 0.78f;

    if (phase < riseEnd) return phase / riseEnd;
    if (phase >= fadeEnd) return 0.0f;

    const float t = 1.0f - (phase - riseEnd) / (fadeEnd - riseEnd);
    return t * t;
}
