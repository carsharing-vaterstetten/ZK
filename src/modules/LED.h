#pragma once

#include <Adafruit_NeoPixel.h>
#include <algorithm>
#include <atomic>
#include <cmath>

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

inline uint32_t getStatusColorValue(const StatusColor color)
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

/// How a driven LED command paints the strip.
enum class LedEffect : uint8_t
{
    /// `progress` as a bar filling across the strip. Reads as "the device is
    /// working, wait" — it is what the boot sequence uses for transfers.
    Bar,

    /// The whole strip breathing. Reads as "waiting on you", which is what an
    /// instruction to the user needs to look like; a steady light reads as
    /// status and gets ignored. Ignores `progress`.
    Pulse,

    /// The whole strip at a uniform `progress` brightness. For dimming away
    /// after the user has done what was asked — a bar draining pixel by pixel
    /// reads as a measurement of something, where a fade just reads as the
    /// light going out.
    Fade,
};

struct ProgressState
{
    float progress = 0;
    uint32_t colorHex = 0xFFFFFF;
    LedEffect effect = LedEffect::Bar;
};

inline uint32_t scaleColorBrightness(const uint32_t colorHex, const float factor)
{
    const float clamped = std::clamp(factor, 0.0f, 1.0f);

    const auto scale = [clamped](const uint32_t channel)
    {
        return static_cast<uint32_t>(static_cast<float>(channel) * clamped + 0.5f) & 0xFFu;
    };

    return scale(colorHex >> 16 & 0xFFu) << 16 | scale(colorHex >> 8 & 0xFFu) << 8 | scale(colorHex & 0xFFu);
}

class StatefulLed : public LED
{
public:
    using LED::LED;

    /// Refresh rate for the time-driven effects. Fast enough that a fade reads as
    /// continuous rather than stepped.
    static constexpr TickType_t animationFrameInterval = pdMS_TO_TICKS(20);

    void loadSequence(StatefulSequencePlayer* player)
    {
        activePlayer = player;
    }

    void stopActiveSequence()
    {
        activePlayer = nullptr;
    }

    StatefulSequencePlayer* getActiveSequence() const
    {
        return activePlayer;
    }

    bool nextState(TickType_t& timeToNextState)
    {
        if (activePlayer == nullptr) return false;
        if (activePlayer->isCompleted())
        {
            activePlayer = nullptr;
            return false;
        }

        return activePlayer->moveToNextState(timeToNextState);
    }

    /// A dot travelling around the strip, trailing a short fade behind it.
    ///
    /// On a single-pixel board there is nowhere for the dot to travel, so it is
    /// rendered passing the one LED instead: a quick rise, a long fade, then a
    /// dark gap before it comes round again. That keeps the "something is
    /// cycling" reading, where a positional spinner just collapses into an
    /// on/off blink that says nothing.
    void renderSpinner(const uint32_t colorHex) const
    {
        const uint16_t pixelCount = neo.numPixels();

        neo.setBrightness(255);

        if (pixelCount <= 1)
        {
            neo.setPixelColor(0, scaleColorBrightness(colorHex, passingCometBrightness(
                                                          animationPhase(singlePixelRevolutionMs))));
            neo.show();
            return;
        }

        // Fractional, so the head glides between LEDs instead of stepping.
        const float head = animationPhase(pixelCount * spinnerMsPerPixel) * pixelCount;

        for (uint16_t i = 0; i < pixelCount; ++i)
        {
            // Distance behind the head, wrapped so the tail follows it around.
            float behind = head - static_cast<float>(i);
            if (behind < 0.0f) behind += static_cast<float>(pixelCount);

            neo.setPixelColor(i, scaleColorBrightness(colorHex, tailBrightness(behind)));
        }

        neo.show();
    }

    [[nodiscard]] StatefulSequencePlayer loadingCircle(uint32_t color = 0xFFFFFF) const
    {
        return StatefulSequencePlayer({
            SequencePoint{
                0, [this, color] { renderSpinner(color); }
            }
        }, [this] { clear(); }, 0, true, true, animationFrameInterval);
    }

    void renderProgressBar(const float progress, const uint32_t colorHex) const
    {
        const uint16_t pixelCount = neo.numPixels();
        const float filled = std::clamp(progress, 0.0f, 1.0f) * static_cast<float>(pixelCount);

        // Global brightness stays out of the way; dimming happens in the colour.
        neo.setBrightness(255);

        for (uint16_t i = 0; i < pixelCount; ++i)
        {
            // How much of this pixel the bar covers: 1 fully, 0 not at all.
            const float coverage = std::clamp(filled - static_cast<float>(i), 0.0f, 1.0f);
            neo.setPixelColor(i, scaleColorBrightness(colorHex, coverage));
        }

        neo.show();
    }

    /// Whole strip at one brightness.
    void renderFade(const float level, const uint32_t colorHex) const
    {
        neo.setBrightness(255);
        neo.fill(scaleColorBrightness(colorHex, perceptual(level)));
        neo.show();
    }

    /// Breathes the whole strip. Self-timed off the tick count so it animates at
    /// the sequence's own refresh rate rather than depending on how often the
    /// task driving it happens to push new state.
    void renderPulse(const uint32_t colorHex) const
    {
        // Slow enough to read as breathing rather than blinking, and never fully
        // dark — going to black would read as a fault indicator.
        constexpr uint32_t periodMs = 1400;
        constexpr float floorBrightness = 0.15f;

        // Raised cosine, so the turning points at both ends are gentle. Symmetric
        // and never dark, which is what keeps this distinct from the spinner's
        // rise-fade-gap even when the two happen to share a colour.
        const float wave = 0.5f * (1.0f - cosf(2.0f * static_cast<float>(PI) * animationPhase(periodMs)));

        renderFade(floorBrightness + (1.0f - floorBrightness) * wave, colorHex);
    }

    void renderState(const ProgressState& state) const
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

    StatefulSequencePlayer progressIndicator(std::shared_ptr<ProgressState> state) const
    {
        return StatefulSequencePlayer({
            SequencePoint{
                0, [this, state] { renderState(*state); }
            }
        }, [this] { clear(); }, 0, true, true, animationFrameInterval);
    }

    [[nodiscard]] StatefulSequencePlayer unlockFlash() const
    {
        return StatefulSequencePlayer({
                                          SequencePoint{0, [this] { neo.setBrightness(255); setColor(0x00FF00); }},
                                          SequencePoint{100, [this] { clear(); }},
                                          SequencePoint{200, [this] { setColor(0x00FF00); }},
                                      }, [this] { clear(); }, pdMS_TO_TICKS(100), false);
    }

    [[nodiscard]] StatefulSequencePlayer lockFlash() const
    {
        return StatefulSequencePlayer({
                                          SequencePoint{0, [this] { neo.setBrightness(255);setColor(0xFF0000); }},
                                          SequencePoint{100, [this] { clear(); }},
                                          SequencePoint{200, [this] { setColor(0xFF0000); }},
                                      },  [this] { clear(); }, pdMS_TO_TICKS(100), false);
    }

    [[nodiscard]] StatefulSequencePlayer cardDeclinedFlash() const
    {
        return StatefulSequencePlayer({
                                          SequencePoint{0, [this] { neo.setBrightness(255);setColor(0xFF0000); }},
                                      }, [this] { clear(); },  pdMS_TO_TICKS(2000), false);
    }

    [[nodiscard]] StatefulSequencePlayer setColorSeq(uint32_t color) const
    {
        return StatefulSequencePlayer({
                                          SequencePoint{0, [this,color] { neo.setBrightness(255);setColor(color); }},
                                      }, [this] { clear(); }, 0, true);
    }

private:
    /// Preserves the original spinner cadence on multi-pixel boards.
    static constexpr uint32_t spinnerMsPerPixel = 100;

    /// One pixel cannot carry a position, so the dot is timed to pass by at a
    /// rate the eye reads as a sweep. Much faster and it becomes a blink again.
    static constexpr uint32_t singlePixelRevolutionMs = 1000;

    /// Tail length in pixels. Short: on a four-pixel ring a long tail lights most
    /// of the strip at once and the dot stops reading as a dot.
    static constexpr float spinnerTailPixels = 1.8f;

    /// Position within one revolution, 0..1. The modulo is taken in integer
    /// before converting, because a float stops representing the tick count
    /// exactly after a few hours at 1kHz and the animation would start to stutter
    /// on a device that has been up since the last nightly restart.
    static float animationPhase(const uint32_t periodMs)
    {
        return static_cast<float>(xTaskGetTickCount() * portTICK_PERIOD_MS % periodMs) / static_cast<float>(periodMs);
    }

    /// Perceptual correction for whole-strip brightness. The eye's response to
    /// LED duty cycle is roughly a square root, so a linear ramp looks like it
    /// collapses at the start and then crawls. Squaring on the way out makes a
    /// fade look even.
    static float perceptual(const float level)
    {
        return level * level;
    }

    /// Brightness of a pixel sitting `behind` pixels behind the comet head.
    /// Squared so the head stays crisp and the trail falls away quickly.
    static float tailBrightness(const float behind)
    {
        if (behind >= spinnerTailPixels) return 0.0f;

        const float t = 1.0f - behind / spinnerTailPixels;
        return t * t;
    }

    /// Single-pixel comet: rise, fade, gap. The gap matters — without it the
    /// fade runs straight into the next rise and the result is a throb rather
    /// than something passing by.
    static float passingCometBrightness(const float phase)
    {
        constexpr float riseEnd = 0.12f;
        constexpr float fadeEnd = 0.78f;

        if (phase < riseEnd) return phase / riseEnd;
        if (phase >= fadeEnd) return 0.0f;

        const float t = 1.0f - (phase - riseEnd) / (fadeEnd - riseEnd);
        return t * t;
    }

    StatefulSequencePlayer* activePlayer = nullptr;
};
