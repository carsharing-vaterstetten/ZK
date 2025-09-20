#pragma once
#include <SPI.h>

class HardwareManager
{
    static bool nfcSpiInitialized;
    static bool sdSpiInitialized;

public:
    static void ensureNFCSPIInitialized();
    static void ensureSDSPIInitialized();

    static SPIClass* nfcSpi;
    static SPIClass* sdSpi;
};
