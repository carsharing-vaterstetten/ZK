#pragma once
#include <atomic>

#include "system/SystemThread.h"
#include "system/SystemManager.h"
#include "hal/NfcReader.h"

struct ScanResult
{
    TickType_t ts;
    uint32_t uid;
};

class CardReaderTask : public SystemThread
{
public:
    explicit CardReaderTask(NfcReader& cardReader) : SystemThread(SystemThreadId::CardReaderTask, "CRS", 4096,
                                                                         ThreadPriority::CardReaderTask, 0),
                                                            cardReader(cardReader)
    {
        SystemManager::RegisterThread(this);
    }

    [[nodiscard]] std::optional<ScanResult> waitForScanResult(TickType_t timeout) const;

protected:
    void setup() override;
    void run() override;

private:
    NfcReader& cardReader;

    QueueHandle_t scanResultQueue = xQueueCreate(1, sizeof(ScanResult));
};
