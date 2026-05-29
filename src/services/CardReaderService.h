#pragma once
#include <atomic>

#include "ServiceTypese.h"
#include "SystemThread.h"
#include "logic/SystemManager.h"
#include "modules/NFCCardReader.h"

class CardReaderService : public SystemThread
{
public:
    static constexpr CommandId newCardScannedId = 54234;

    explicit CardReaderService(NFCCardReader& cardReader) : SystemThread(SystemThreadId::CardReaderService, "CRS", 4096,
                                                                         1),
                                                            cardReader(cardReader)
    {
        SystemManager::RegisterThread(this);
    }

    void OnCommand(SystemCommand cmd) override;

protected:
    void setup() override;
    void run() override;

private:
    std::atomic<bool> m_running = true;

    NFCCardReader& cardReader;
};
