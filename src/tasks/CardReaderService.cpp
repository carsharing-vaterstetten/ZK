#include "tasks/CardReaderService.h"

void CardReaderService::OnCommand(const SystemCommand cmd)
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

std::optional<ScanResult> CardReaderService::waitForScanResult(const TickType_t timeout) const
{
    ScanResult result{};
    if (xQueueReceive(scanResultQueue, &result, timeout) == pdTRUE)
        return result;
    return std::nullopt;
}

void CardReaderService::setup()
{
    cardReader.connect();
}

void CardReaderService::run()
{
    ScanResult scanResult{};

    while (m_running)
    {
        std::optional<uint32_t> uid = cardReader.scan();

        if (!uid.has_value())
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        scanResult.uid = uid.value();
        scanResult.ts = xTaskGetTickCount();
        xQueueOverwrite(scanResultQueue, &scanResult);
    }

    SystemManager::ReportReadyForRestart(m_id);
}
