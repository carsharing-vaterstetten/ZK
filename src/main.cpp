#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1 KiB

#include "Device.h"
#include "Platform.h"

void setup()
{
    Platform::begin();

    // Function-local static: constructed once, never destroyed, and lives in
    // .bss rather than on the setup() stack.
    static Device device{Platform::selectBoard(), Platform::loadConfig()};
    device.begin();
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
