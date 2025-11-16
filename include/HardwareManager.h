#pragma once
#include <SPI.h>

#include "Config.h"

namespace HardwareManager
{
    inline SPIClass nfcSpi{NFC_SPI};

    inline void begin()
    {
        nfcSpi.begin(NFC_SCLK, NFC_MISO, NFC_MOSI);
    }
}
