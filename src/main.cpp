#include <Arduino.h>
#include <Modem.h>
#include <NFC.h>
#include <SPIFFSUtils.h>
#include <HelperUtils.h>
#include <LED.h>
#include <esp32-hal.h>
#include <esp_system.h>
#include <esp_task_wdt.h>

#define HW_WATCHDOG_TIMEOUT (300) // [Seconds], 5mins
#define HW_WATCHDOG_RESET_DELAY_MS (100) // [Milliseconds]

bool loggedIn = false;
unsigned long nextWatchdogResetMs;
unsigned long targetMillis;
const unsigned long dayMillis = 24 * 60 * 60 * 1000; // Ein Tag in Millisekunden
String MAC_ADDRESS;

Modem modem;
NFC nfc;
LED LED_Strip;
Config config;

void initKeyPins()
{
    pinMode(OPEN_KEY, OUTPUT);
    pinMode(CLOSE_KEY, OUTPUT);
}

// Funktion überprüft, ob die gescannte Karte in der Speicherdatei vorhanden ist.
// Wenn ja, wird je nach Zustand loggedIn auf true oder false gesetzt.
void checkNFCTag()
{
    String readValue = nfc.readTag();
    if (readValue != "")
    {
        Serial.println(readValue);
        if (SPIFFSUtils::isRfidInSPIFFS(readValue))
        {
            loggedIn = !loggedIn;
            if (loggedIn)
            {
                digitalWrite(OPEN_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("green");
                digitalWrite(OPEN_KEY, LOW);
                SPIFFSUtils::addLogEntry("Auto aufgesperrt");
            }
            else
            {
                digitalWrite(CLOSE_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("red");
                digitalWrite(CLOSE_KEY, LOW);
                SPIFFSUtils::addLogEntry("Auto zugesperrt");
            }
            delay(2000);
        }
        else
        {
            SPIFFSUtils::addLogEntry("Unbekannte RFID-Karte gescannt");
            LED_Strip.setStaticColor("red");
            delay(2000);
        }
        LED_Strip.clear();
    }
}

void initTime()
{

    String time = modem.getLocalTime();
    Serial.println("Local Time: " + time);

    // Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    String hourStr = time.substring(9, 11);
    String minStr = time.substring(12, 14);
    String secStr = time.substring(15, 17);

    int hour = hourStr.toInt();
    int minute = minStr.toInt();
    int second = secStr.toInt();

    // Berechne die Millisekunden seit Mitternacht
    unsigned long currentMillis = (hour * 3600 + minute * 60 + second) * 1000;

    if (currentMillis < targetTimeToRestartESP32)
    {
        targetMillis = targetTimeToRestartESP32 - currentMillis;
    }
    else
    {
        targetMillis = dayMillis - (currentMillis - targetTimeToRestartESP32);
    }

    targetMillis += millis(); // Setze Zielzeit relativ zu millis()
}

void initWatchdog() {
    // Set hw watchdog timeout. The watchdog will reset the device after this timeout

    Serial.print("Initializing the Task Watchdog Timer... ");
    const esp_err_t watchdog_init_err = esp_task_wdt_init(HW_WATCHDOG_TIMEOUT, true);

    switch (watchdog_init_err) {
        case ESP_OK:
            Serial.println("Success");
            break;
        case ESP_ERR_NO_MEM:
            Serial.println("Failed due to lack of memory!");
            break;
        default:
            Serial.println("Unknown status!");
            break;
    }

    // Add the current task (main loop) to be monitored by the watchdog.
    // This ensures that if the main loop doesn't reset the watchdog in time,
    // the ESP32 will reset itself.

    Serial.print("Subscribing the current task to the Task Watchdog Timer... ");

    const esp_err_t error_code = esp_task_wdt_add(nullptr);

    switch (error_code) {
        case ESP_OK:
            Serial.println("Success");
            break;
        case ESP_ERR_INVALID_ARG:
            Serial.println("Error, the task is already subscribed!");
            break;
        case ESP_ERR_NO_MEM:
            Serial.println("Error, could not subscribe the task due to lack of memory!");
            break;
        case ESP_ERR_INVALID_STATE:
            Serial.println("Error, the Task Watchdog Timer has not been initialized yet!");
            break;
        default:
            Serial.println("Unknown status!");
            break;
    }
}

void setup()
{
    Serial.begin(UART_BAUD);
    while (!Serial)
    {
    };

    const esp_reset_reason_t reset_reason = esp_reset_reason();

    Serial.print("ESP32 startup. Reset Reason: ");
    Serial.println(HelperUtils::getResetReasonHumanReadable(reset_reason));

    // Initialize the watchdog.
    // WARNING: If this setup function does not complete within the given HW_WATCHDOG_TIMEOUT the watchdog will perform a reset.
    // That could possibly lead to an infinite resetting loop.
    initWatchdog();

    HelperUtils::initEEPROM(config);

    if (!HelperUtils::loadConfigFromEEPROM(config))
    {
        delay(5000);
        Serial.println("Bitte Konfigurationsdaten eingeben:");
        Serial.println("Format: apn=\"\";gprsUser=\"\";gprsPass=\"\";GSM_PIN=\"\";server=\"zk.de\";port=443;username=\"\";password=\"\";");

        String inputString = "";
        while (inputString.length() == 0)
        {
            if (Serial.available())
            {
                inputString = Serial.readStringUntil('\n');
            }
        }
        HelperUtils::parseConfigString(inputString, config);
        HelperUtils::saveConfigToEEPROM(config);
    }

    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount Failed.");
        if (!SPIFFS.format()) {
            Serial.println("Format ging auch nicht! Tja");
        }
    }
    initKeyPins();

    LED_Strip.init();
    LED_Strip.setStaticColor("white");

    MAC_ADDRESS = HelperUtils::getMacAddress();
    
    modem.init();
    initTime();

    SPIFFSUtils::addLogEntry("ESP32 wird initialisiert");

    LED_Strip.setStaticColor("purple");
    modem.firmwareCheckAndUpdateIfNeeded();

    LED_Strip.setStaticColor("orange");

    int arraySize;
    String *rfids = modem.getRfids(arraySize);

    if (rfids != nullptr)
    {
        SPIFFSUtils::saveRfidsToSPIFFS(rfids, arraySize);
        delete[] rfids;
    }

    nfc.init();

    SPIFFSUtils::addLogEntry("ESP32 wurde fertig initialisiert");

    LED_Strip.setStaticColor("blue");
    modem.uploadLogs();

    LED_Strip.clear();
}

void loop() {
    if (millis() >= nextWatchdogResetMs) {
        esp_task_wdt_reset(); // Reset the watchdog timer
        nextWatchdogResetMs = millis() + HW_WATCHDOG_RESET_DELAY_MS;
    }

    if (millis() >= targetMillis) {
        Serial.println("Target time reached.");
        Serial.println("Uploading logs and restarting ESP32...");
        SPIFFSUtils::addLogEntry("ESP32 wird neu gestartet");
        LED_Strip.setStaticColor("blue");
        while (!modem.uploadLogs())
        {
            delay(100);
        }
        ESP.restart(); // ESP32 neustarten
    }
    checkNFCTag();
}
