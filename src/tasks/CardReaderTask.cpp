#include "tasks/CardReaderTask.h"

std::optional<ScanResult> CardReaderTask::waitForScanResult(const TickType_t timeout) const
{
    ScanResult result{};
    if (xQueueReceive(scanResultQueue, &result, timeout) == pdTRUE)
        return result;
    return std::nullopt;
}

void CardReaderTask::setup()
{
    cardReader.connect();
}

void CardReaderTask::run()
{
    ScanResult scanResult{};

    while (isRunning())
    {
        std::optional<uint32_t> uid = cardReader.scan();

        if (!uid.has_value())
        {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        scanResult.uid = uid.value();
        scanResult.ts = xTaskGetTickCount();
        xQueueOverwrite(scanResultQueue, &scanResult);
    }
}
