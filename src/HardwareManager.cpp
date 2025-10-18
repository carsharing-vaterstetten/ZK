#include "HardwareManager.h"

#include "Config.h"

SPIClass* HardwareManager::nfcSpi = nullptr;
bool HardwareManager::nfcSpiInitialized = false;

void HardwareManager::ensureNFCSPIInitialized()
{
    if (nfcSpiInitialized) return;

    delete nfcSpi;
    nfcSpi = new SPIClass(NFC_SPI);
    nfcSpi->begin(NFC_SCLK, NFC_MISO, NFC_MOSI);
    nfcSpiInitialized = true;
}
