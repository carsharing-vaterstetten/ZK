#pragma once
#include <atomic>

#include "SystemThread.h"
#include "logic/SystemManager.h"
#include "modules/NFCCardReader.h"

struct ScanResult
{
    TickType_t ts;
    uint32_t uid;
};

class CardReaderService : public SystemThread
{
public:
    explicit CardReaderService(NFCCardReader& cardReader) : SystemThread(SystemThreadId::CardReaderService, "CRS", 4096,
                                                                         ThreadPriority::CardReaderService, 0),
                                                            cardReader(cardReader)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;
    [[nodiscard]] std::optional<ScanResult> waitForScanResult(TickType_t timeout) const;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    NFCCardReader& cardReader;

    QueueHandle_t scanResultQueue = xQueueCreate(1, sizeof(ScanResult));
};
