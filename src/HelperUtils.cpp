// HelperUtils.cpp

#include "HelperUtils.h"

String HelperUtils::toUpperCase(const String &str)
{
    String upperStr = str;
    for (int i = 0; i < upperStr.length(); i++)
    {
        upperStr[i] = toupper(upperStr[i]);
    }
    return upperStr;
}

String HelperUtils::getMacAddress()
{
    byte mac[6];
    WiFi.macAddress(mac);
    String macStr = "";
    for (int i = 0; i < 6; i++)
    {
        if (mac[i] < 0x10)
        {
            macStr += '0';
        }
        macStr += String(mac[i], HEX);
        if (i < 5)
        {
            macStr += ':';
        }
    }
    return HelperUtils::toUpperCase(macStr);
}
