// HelperUtils.h
#ifndef HELPERUTILS_H
#define HELPERUTILS_H

#include <Arduino.h>
#include <Config.h>
#include <Intern.h>
#include <WiFi.h>
#include <EEPROM.h>

class HelperUtils
{
public:
    static String getMacAddress();
    static String toUpperCase(const String &str);
    static void initEEPROM(Config &config);
    static void saveConfigToEEPROM(Config &config);
    static bool loadConfigFromEEPROM(Config &config);
    static void parseConfigString(String &inputString, Config &config);
    static void resetEEPROM();
    static String getResetReasonHumanReadable(esp_reset_reason_t reset_reason);
    static esp_err_t setWatchdog(uint32_t watchdog_timeout);
    static esp_err_t subscribeTaskToWatchdog();
};

#endif
