#include "HardwareManager.h"

#include "Config.h"

SPIClass* HardwareManager::nfcSpi = nullptr;
SPIClass* HardwareManager::sdSpi = nullptr;
bool HardwareManager::nfcSpiInitialized = false;
bool HardwareManager::sdSpiInitialized = false;

void HardwareManager::ensureNFCSPIInitialized()
{
    if (nfcSpiInitialized) return;

    delete nfcSpi;
    nfcSpi = new SPIClass(NFC_SPI);
    nfcSpi->begin(NFC_SCLK, NFC_MISO, NFC_MOSI);
    nfcSpiInitialized = true;
}

void HardwareManager::ensureSDSPIInitialized()
{
    if (sdSpiInitialized) return;

    delete sdSpi;
    sdSpi = new SPIClass(SD_SPI);
    sdSpi->begin(SD_SCLK, SD_MISO, SD_MOSI);
    sdSpiInitialized = true;
}
