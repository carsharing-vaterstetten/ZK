// HelperUtils.cpp

#include "HelperUtils.h"

#include <esp_task_wdt.h>

String HelperUtils::toUpperCase(const String &str) {
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

void HelperUtils::initEEPROM(Config &config)
{
    EEPROM.begin(sizeof(Config));
}

void HelperUtils::saveConfigToEEPROM(Config &config)
{
    config.version = CONFIG_VERSION;
    EEPROM.put(CONFIG_START_ADDRESS, config);
    EEPROM.commit();
    Serial.println("Konfiguration wurde gespeichert.");
}

bool HelperUtils::loadConfigFromEEPROM(Config &config)
{
    EEPROM.get(CONFIG_START_ADDRESS, config);
    if (config.version != CONFIG_VERSION)
    {
        return false; 
    }
    return true;
}

void HelperUtils::parseConfigString(String &inputString, Config &config) {
  // String anhand von ';' aufteilen
  int start = 0;
  while (start < inputString.length()) {
    int end = inputString.indexOf(';', start);
    if (end == -1) {
      end = inputString.length();
    }
    String token = inputString.substring(start, end);
    // Token verarbeiten, sollte in der Form key="value" oder key=value sein
    int eqIndex = token.indexOf('=');
    if (eqIndex != -1) {
      String key = token.substring(0, eqIndex);
      String value = token.substring(eqIndex + 1);
      value.trim();
      if (value.startsWith("\"") && value.endsWith("\"")) {
        value = value.substring(1, value.length() - 1);
      }
      key.trim();
      if (key == "apn") {
        value.toCharArray(config.apn, sizeof(config.apn));
      } else if (key == "gprsUser") {
        value.toCharArray(config.gprsUser, sizeof(config.gprsUser));
      } else if (key == "gprsPass") {
        value.toCharArray(config.gprsPass, sizeof(config.gprsPass));
      } else if (key == "GSM_PIN") {
        value.toCharArray(config.GSM_PIN, sizeof(config.GSM_PIN));
      } else if (key == "server") {
        value.toCharArray(config.server, sizeof(config.server));
      } else if (key == "port") {
        config.port = value.toInt();
      } else if (key == "username") {
        value.toCharArray(config.username, sizeof(config.username));
      } else if (key == "password") {
        value.toCharArray(config.password, sizeof(config.password));
      } else {
        Serial.print("Unbekannter Schlüssel: ");
        Serial.println(key);
      }
    }
    start = end + 1;
  }
}

// Funktion zum Zurücksetzen des EEPROM
void HelperUtils::resetEEPROM() {
  Config emptyConfig;
  emptyConfig.version = 0xFF; 
  EEPROM.put(CONFIG_START_ADDRESS, emptyConfig);
  EEPROM.commit();
  Serial.println("EEPROM wurde zurückgesetzt.");
}

String HelperUtils::getResetReasonHumanReadable(const esp_reset_reason_t reset_reason) {
    switch (reset_reason) {
        case ESP_RST_POWERON:
            return "Reset due to power-on event";
        case ESP_RST_BROWNOUT:
            return "Brownout reset";
        case ESP_RST_SDIO:
            return "Reset over SDIO";
        case ESP_RST_PANIC:
            return "Software reset due to exception/panic";
        case ESP_RST_INT_WDT:
            return "Reset due to interrupt watchdog";
        case ESP_RST_TASK_WDT:
            return "Reset due to task watchdog";
        case ESP_RST_WDT:
            return "General watchdog reset";
        case ESP_RST_DEEPSLEEP:
            return "Reset after exiting deep sleep mode";
        case ESP_RST_EXT:
            return "Reset by external pin";
        case ESP_RST_UNKNOWN:
            return "Reset reason could not be determined";
        case ESP_RST_SW:
            return "Software reset via esp_restart";
        default:
            return "Unknown reset reason";
    }
}

/// This function configures and initializes the TWDT. If the TWDT is already initialized when this function is called, this function will update the TWDT's timeout period
/// @param watchdog_timeout Timeout period of TWDT in seconds
esp_err_t HelperUtils::setWatchdog(const uint32_t watchdog_timeout) {
    Serial.print("Initializing the Task Watchdog Timer... ");
    const esp_err_t watchdog_init_err = esp_task_wdt_init(watchdog_timeout, true);

    switch (watchdog_init_err) {
        case ESP_OK:
            Serial.println("Success");
            break;
        case ESP_ERR_NO_MEM:
            Serial.println("Failed due to lack of memory!");
            return watchdog_init_err;
        default:
            Serial.println("Unknown status!");
            return watchdog_init_err;
    }

    return ESP_OK;
}

esp_err_t HelperUtils::subscribeTaskToWatchdog() {
    Serial.print("Subscribing the current task to the Task Watchdog Timer... ");

    const esp_err_t task_wdt_add_error = esp_task_wdt_add(nullptr);

    switch (task_wdt_add_error) {
        case ESP_OK:
            Serial.println("Success");
            break;
        case ESP_ERR_INVALID_ARG:
            Serial.println("Error, the task is already subscribed!");
            return task_wdt_add_error;
        case ESP_ERR_NO_MEM:
            Serial.println("Error, could not subscribe the task due to lack of memory!");
            return task_wdt_add_error;
        case ESP_ERR_INVALID_STATE:
            Serial.println("Error, the Task Watchdog Timer has not been initialized yet!");
            return task_wdt_add_error;
        default:
            Serial.println("Unknown status!");
            return task_wdt_add_error;
    }

    return ESP_OK;
}
