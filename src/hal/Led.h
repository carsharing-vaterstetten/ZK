#pragma once

#include <Adafruit_NeoPixel.h>
#include <algorithm>
#include <cmath>

#include "util/SequencePlayer.h"

/// How a driven LED command paints the strip.
enum class LedEffect : uint8_t
{
    /// `progress` as a bar filling across the strip. Reads as "working, wait" —
    /// it is what the boot sequence uses for transfers.
    Bar,

    /// The whole strip breathing. Reads as an instruction to the user, where a
    /// steady light reads as status and gets ignored. Ignores `progress`.
    Pulse,

    /// The whole strip at a uniform `progress` brightness, for dimming away once
    /// the user has done what was asked.
    Fade,
};

struct ProgressState
{
    float progress = 0;
    uint32_t colorHex = 0xFFFFFF;
    LedEffect effect = LedEffect::Bar;
};

/// Scales an 0xRRGGBB colour by `factor` (0..1). Per pixel rather than via
/// Adafruit_NeoPixel::setBrightness, which rescales the whole buffer in place and
/// loses a little precision on every call.
inline uint32_t scaleColorBrightness(const uint32_t colorHex, const float factor)
{
    const float clamped = std::clamp(factor, 0.0f, 1.0f);

    const auto scale = [clamped](const uint32_t channel)
    {
        return static_cast<uint32_t>(static_cast<float>(channel) * clamped + 0.5f) & 0xFFu;
    };

    return scale(colorHex >> 16 & 0xFFu) << 16 | scale(colorHex >> 8 & 0xFFu) << 8 | scale(colorHex & 0xFFu);
}

/// Renders to the strip and builds the sequences that drive it. Owns no playback
/// state — LedSchedulerTask decides what plays and when.
class Led
{
public:
    explicit Led(Adafruit_NeoPixel& ledDriver);

    /// Refresh rate for the time-driven effects, fast enough that a fade reads as
    /// continuous rather than stepped.
    static constexpr TickType_t animationFrameInterval = pdMS_TO_TICKS(20);

    void setColor(uint32_t hex) const;
    void clear() const;

    void renderState(const ProgressState& state) const;

    /// A dot travelling around the strip with a short fade behind it. A
    /// single-pixel board has nowhere for the dot to travel, so it is rendered
    /// passing the one LED instead — rise, fade, gap — rather than collapsing
    /// into the on/off blink a positional spinner degrades to.
    void renderSpinner(uint32_t colorHex) const;

    [[nodiscard]] StatefulSequencePlayer loadingCircle(uint32_t color = 0xFFFFFF) const
    {
        return StatefulSequencePlayer({
            SequencePoint{0, [this, color] { renderSpinner(color); }}
        }, [this] { clear(); }, 0, true, true, animationFrameInterval);
    }

    [[nodiscard]] StatefulSequencePlayer progressIndicator(std::shared_ptr<ProgressState> state) const
    {
        return StatefulSequencePlayer({
            SequencePoint{0, [this, state] { renderState(*state); }}
        }, [this] { clear(); }, 0, true, true, animationFrameInterval);
    }

    [[nodiscard]] StatefulSequencePlayer unlockFlash() const { return flash(0x00FF00); }
    [[nodiscard]] StatefulSequencePlayer lockFlash() const { return flash(0xFF0000); }

    [[nodiscard]] StatefulSequencePlayer cardDeclinedFlash() const
    {
        return StatefulSequencePlayer({
            SequencePoint{0, [this] { neo.setBrightness(255); setColor(0xFF0000); }},
        }, [this] { clear(); }, pdMS_TO_TICKS(2000), false);
    }

private:
    /// Preserves the original spinner cadence on multi-pixel boards.
    static constexpr uint32_t spinnerMsPerPixel = 100;

    /// One pixel cannot carry a position, so the dot is timed to pass by at a rate
    /// the eye reads as a sweep. Much faster and it becomes a blink again.
    static constexpr uint32_t singlePixelRevolutionMs = 1000;

    /// Tail length in pixels. Short: on a four-pixel ring a long tail lights most
    /// of the strip and the dot stops reading as a dot.
    static constexpr float spinnerTailPixels = 1.8f;

    Adafruit_NeoPixel& neo;

    /// Blink-off-blink in `color`, ending lit. Both lock and unlock use this.
    [[nodiscard]] StatefulSequencePlayer flash(const uint32_t color) const
    {
        return StatefulSequencePlayer({
            SequencePoint{0, [this, color] { neo.setBrightness(255); setColor(color); }},
            SequencePoint{100, [this] { clear(); }},
            SequencePoint{200, [this, color] { setColor(color); }},
        }, [this] { clear(); }, pdMS_TO_TICKS(100), false);
    }

    void renderProgressBar(float progress, uint32_t colorHex) const;
    void renderFade(float level, uint32_t colorHex) const;
    void renderPulse(uint32_t colorHex) const;

    /// Position within one period, 0..1. The modulo is taken in integer before
    /// converting: a float stops representing the tick count exactly after a few
    /// hours at 1kHz, and the animation would start to stutter.
    static float animationPhase(const uint32_t periodMs)
    {
        return static_cast<float>(xTaskGetTickCount() * portTICK_PERIOD_MS % periodMs) / static_cast<float>(periodMs);
    }

    /// Perceptual correction. Eye response to LED duty cycle is roughly a square
    /// root, so a linear ramp looks like it collapses and then crawls.
    static float perceptual(const float level) { return level * level; }

    /// Brightness `behind` pixels behind the comet head.
    static float tailBrightness(float behind);

    /// Single-pixel comet: rise, fade, gap. The gap is what stops it reading as a
    /// throb rather than as something passing by.
    static float passingCometBrightness(float phase);
};
