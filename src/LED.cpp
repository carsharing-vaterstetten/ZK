#include "LED.h"

LED::LED(const uint16_t ledCount, const int16_t ledPin, const neoPixelType type) : neo{ledCount, ledPin, type}
{
}

bool LED::init()
{
    if (!neo.begin()) return false;
    neo.show();
    return true;
}

void LED::setStatusColor(const StatusColor color)
{
    neo.fill(getStatusColorValue(color));
    neo.show();
}

void LED::clear()
{
    neo.clear();
    neo.show();
}

uint32_t LED::getStatusColorValue(const StatusColor color)
{
    switch (color)
    {
    case StatusColor::PerformingOTAUpdate:
        return 0x800080;
    case StatusColor::Error:
        return 0xFF0000;
    case StatusColor::InitializationPhase:
        return 0xFFFFFF;
    case StatusColor::UpdatingRFIDs:
        return 0xFFA500;
    case StatusColor::UploadingLogs:
        return 0x0000FF;
    case StatusColor::CarLocked:
        return 0xFF0000;
    case StatusColor::CarUnlocked:
        return 0x00FF00;
    case StatusColor::NFCUnknownUIDScanned:
        return 0xFF0000;
    default:
        return 0;
    }
}
