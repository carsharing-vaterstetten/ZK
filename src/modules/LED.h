#pragma once

#include <Adafruit_NeoPixel.h>
#include <atomic>

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

struct ProgressState
{
    float progress = 0;
    uint32_t colorHex = 0xFFFFFF;
};

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

    StatefulSequencePlayer progressIndicator(std::shared_ptr<ProgressState> state) const
    {
        return StatefulSequencePlayer({
            SequencePoint{
                0, [this, state]
                {
                    const uint8_t brightness = state->progress * 255.0f;
                    neo.setBrightness(brightness);
                    neo.fill(state->colorHex);
                    neo.show();
                }
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
