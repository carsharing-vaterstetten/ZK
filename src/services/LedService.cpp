#include "LedService.h"

#include "modules/LED.h"

void LedService::OnCommand(SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        break;
    case SystemCommand::EnterLowPower:
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

void LedService::setStateColor(StatusColor color) const
{
    auto a = LedCommandWithData{.cmd = LedCommand::SetState, .data = color};
    xQueueSend(commandQueue, &a, portMAX_DELAY);
}

void LedService::clrStateColor() const
{
    auto a = LedCommandWithData{.cmd = LedCommand::ClearState};
    xQueueSend(commandQueue, &a, portMAX_DELAY);
}

void LedService::playSequence(LedSequence sequence) const
{
    auto a = LedCommandWithData{.cmd = LedCommand::PlaySequence, .data = sequence};
    xQueueSend(commandQueue, &a, portMAX_DELAY);
}

void LedService::setup() {}

void LedService::run()
{
    LedCommandWithData cmdAndData;

    while (m_running)
    {
        if (xQueueReceive(commandQueue, &cmdAndData, pdMS_TO_TICKS(500)) == pdFALSE)
            continue;

        switch (cmdAndData.cmd)
        {
        case LedCommand::PlaySequence:
            {
                switch (std::get<LedSequence>(cmdAndData.data))
                {
                case LedSequence::WaitingForCardRemoval:
                    {
                        const ulong s = millis();
                        while (millis() - s < cardRemovalCooldown)
                        {
                            const float progress = 1.0f - (float)(millis() - s) / (float)cardRemovalCooldown;
                            statusLed.progressIndicatorNext(StatusColor::WaitingForNFCCardToBeRemoved, progress);
                        }
                        statusLed.progressIndicatorStop();
                        break;
                    }
                case LedSequence::CardDeclined:
                    statusLed.cardDeclinedFlash();
                    break;
                case LedSequence::CarUnlocked:
                    statusLed.unlockFlash();
                    break;
                case LedSequence::CarLocked:
                    statusLed.lockFlash();
                    break;
                }

                break;
            }
        case LedCommand::SetState:
            oldStatusColor = std::get<StatusColor>(cmdAndData.data);
            statusLed.setStatusColor(oldStatusColor.value());
            break;
        case LedCommand::ClearState:
            oldStatusColor = std::nullopt;
            statusLed.clear();
            break;
        case LedCommand::None:
            break;
        }

        // Return to previous state (will be newly set status color, if command was not a sequence)
        if (oldStatusColor.has_value())
            statusLed.setStatusColor(oldStatusColor.value());
        else
            statusLed.clear();
    }

    SystemManager::ReportReadyForRestart(m_id);
}
