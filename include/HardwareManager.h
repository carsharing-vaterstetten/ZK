#pragma once
#include <SPI.h>

class HardwareManager
{
    static bool nfcSpiInitialized;

public:
    static void ensureNFCSPIInitialized();

    static SPIClass* nfcSpi;
};
