// HelperUtils.cpp

#include "HelperUtils.h"
#include "Globals.h"
#include <EEPROM.h>
#include "mbedtls/md5.h"
#include <esp_task_wdt.h>
#include <FS.h>


void HelperUtils::parseConfigString(const String& inputString, Config& c)
{
    // String anhand von ';' aufteilen
    int start = 0;

    while (start < inputString.length())
    {
        int end = inputString.indexOf(';', start);

        if (end == -1)
        {
            end = inputString.length();
        }

        String token = inputString.substring(start, end);

        // Token verarbeiten, sollte in der Form key="value" oder key=value sein
        const int eqIndex = token.indexOf('=');

        if (eqIndex != -1)
        {
            String key = token.substring(0, eqIndex);
            String value = token.substring(eqIndex + 1);

            value.trim();

            if (value.startsWith("\"") && value.endsWith("\""))
            {
                value = value.substring(1, value.length() - 1);
            }

            key.trim();

            if (key == "apn")
            {
                value.toCharArray(c.apn, sizeof(c.apn));
            }
            else if (key == "gprsUser")
            {
                value.toCharArray(c.gprsUser, sizeof(c.gprsUser));
            }
            else if (key == "gprsPass")
            {
                value.toCharArray(c.gprsPass, sizeof(c.gprsPass));
            }
            else if (key == "GSM_PIN")
            {
                value.toCharArray(c.GSM_PIN, sizeof(c.GSM_PIN));
            }
            else if (key == "server")
            {
                value.toCharArray(c.server, sizeof(c.server));
            }
            else if (key == "port")
            {
                c.port = value.toInt();
            }
            else if (key == "password")
            {
                value.toCharArray(c.password, sizeof(c.password));
            }
            else if (key == "preferSDCard")
            {
                c.preferSDCard = value.toInt();
            }
            else
            {
                fileLog.warningln("Encountered an unknown key while decoding user input config: '" + key + "'");
            }
        }
        start = end + 1;
    }
}

String HelperUtils::getConfigHumanReadable(const Config& c)
{
    return "Config version: " + String(c.version) + " apn=" + c.apn + " gprsUser=" + c.gprsUser +
        " GSM_PIN=" + c.GSM_PIN + " server=" + c.server + " port=" + String(c.port)
        + " password=" + c.password + " preferSDCard=" + String(c.preferSDCard);
}

String HelperUtils::getConfigFormat(const Config& c)
{
    return "apn=\"" + String(c.apn) + "\";gprsUser=\"" + c.gprsUser +
        "\";GSM_PIN=\"" + c.GSM_PIN + "\";server=\"" + c.server + "\";port=\"" + String(c.port) +
        + "\";password=\"" + c.password + "\";preferSDCard=\"" + String(c.preferSDCard) + "\";";
}

String HelperUtils::getResetReasonHumanReadable(const esp_reset_reason_t reset_reason)
{
    switch (reset_reason)
    {
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
esp_err_t HelperUtils::setWatchdog(const uint32_t watchdog_timeout)
{
    fileLog.infoln("Setting TWDT timeout to " + String(watchdog_timeout) + " seconds");
    const esp_err_t watchdog_init_err = esp_task_wdt_init(watchdog_timeout, true);
    fileLog.logInfoOrErrorln(watchdog_init_err == ESP_OK, "TWDT timeout set successfully",
                             "Setting TWDT timeout failed. Error " + String(watchdog_init_err));
    return watchdog_init_err;
}

esp_err_t HelperUtils::subscribeTaskToWatchdog()
{
    const esp_err_t task_wdt_add_error = esp_task_wdt_add(nullptr);
    fileLog.logInfoOrErrorln(task_wdt_add_error == ESP_OK, "Successfully subscribed current task to TWDT.",
                             "Failed to subscribe current task to TWDT. Error " + String(task_wdt_add_error));
    return task_wdt_add_error;
}

void HelperUtils::requestConfig(Config& c)
{
    Serial.println("Please enter config data in this format:");
    constexpr Config exampleConfig = {
        CONFIG_VERSION,
        "iot.1nce.net",
        "",
        "",
        "",
        "example.com",
        80,
        "XXX",
        true,
    };
    Serial.println(getConfigFormat(exampleConfig));

    String inputString = "";

    while (inputString.length() == 0)
    {
        if (Serial.available())
        {
            inputString = Serial.readStringUntil('\n');
        }
    }

    fileLog.infoln("User entered config String: " + inputString);

    parseConfigString(inputString, c);
}

bool HelperUtils::md5File(File file, uint8_t out[16])
{
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);

    uint8_t buf[512];
    while (file.available())
    {
        const size_t len = file.read(buf, sizeof(buf));
        mbedtls_md5_update_ret(&ctx, buf, len);
    }

    mbedtls_md5_finish_ret(&ctx, out);
    mbedtls_md5_free(&ctx);
    file.close();
    return true;
}

String HelperUtils::md5ToHex(const uint8_t md5[16])
{
    String hex;
    for (int i = 0; i < 16; i++)
    {
        char buf[3];
        sprintf(buf, "%02x", md5[i]);
        hex += buf;
    }
    return hex;
}
