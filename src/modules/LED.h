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

    [[nodiscard]] StatefulSequencePlayer loadingCircle(uint32_t color = 0xFFFFFF) const
    {
        auto sequence = std::vector<SequencePoint>(neo.numPixels());

        for (int i = 0; i < neo.numPixels(); i++)
        {
            sequence[i] = SequencePoint{
                100U * i, [this,i,color]
                {
                    neo.clear();
                    neo.setPixelColor(i, color);
                    neo.show();
                }
            };
        }

        return StatefulSequencePlayer{sequence, [this] { clear(); }, 0, true, true, pdMS_TO_TICKS(100)};
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

    /// Breathes the whole strip. Self-timed off the tick count so it animates at
    /// the sequence's own refresh rate rather than depending on how often the
    /// task driving it happens to push new state.
    void renderPulse(const uint32_t colorHex) const
    {
        // Slow enough to read as breathing rather than blinking, and never fully
        // dark — going to black would read as a fault indicator.
        constexpr uint32_t periodMs = 1400;
        constexpr float floorBrightness = 0.15f;

        // Integer modulo before converting: a float holds the tick count exactly
        // for only a few hours at 1kHz, after which the phase would quantise.
        const uint32_t phaseMs = xTaskGetTickCount() * portTICK_PERIOD_MS % periodMs;
        const float phase = static_cast<float>(phaseMs) / periodMs;

        // Raised cosine, so the turning points at both ends are gentle.
        const float wave = 0.5f * (1.0f - cosf(2.0f * static_cast<float>(PI) * phase));

        neo.setBrightness(255);
        neo.fill(scaleColorBrightness(colorHex, floorBrightness + (1.0f - floorBrightness) * wave));
        neo.show();
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
        }
    }

    StatefulSequencePlayer progressIndicator(std::shared_ptr<ProgressState> state) const
    {
        return StatefulSequencePlayer({
            SequencePoint{
                0, [this, state] { renderState(*state); }
            }
        }, [this] { clear(); }, 0, true, true, pdMS_TO_TICKS(20));
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
    StatefulSequencePlayer* activePlayer = nullptr;
};
