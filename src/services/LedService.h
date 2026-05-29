#pragma once

#include <atomic>
#include <variant>

#include "SystemThread.h"
#include "logic/SystemManager.h"
#include "modules/LED.h"

using LedAction = std::function<void()>;

enum class LedSequence
{
    WaitingForCardRemoval,
    CardDeclined,
    CarUnlocked,
    CarLocked,
};

enum class LedState
{
    Off,
    StateSet,
    SequencePlaying,
};

enum class LedCommand
{
    PlaySequence,
    SetState,
    ClearState,
    None,
};

struct LedCommandWithData
{
    LedCommand cmd;
    std::variant<StatusColor, LedSequence> data;
};

class LedService : public SystemThread
{
public:
    LedService(CardReaderLED& statusLed) : SystemThread(SystemThreadId::LedService, "LEDSER", 4096, 3), statusLed(statusLed)
    {
        SystemManager::RegisterThread(this);
    }

    void setStateColor(StatusColor color) const;
    void clrStateColor() const;
    void playSequence(LedSequence sequence) const;

protected:
    void run() override;

private:
    std::atomic<bool> m_running = true;

    LedState currentState = LedState::Off;
    std::optional<StatusColor> oldStatusColor = std::nullopt;

    CardReaderLED& statusLed;

    QueueHandle_t commandQueue = xQueueCreate(10, sizeof(LedCommandWithData));
};
