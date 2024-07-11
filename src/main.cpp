#include <Arduino.h>
#include <Modem.h>
#include <NFC.h>
#include <SPIFFSUtils.h>

#define SerialMon Serial

bool loggedIn = false;

Modem modem;
NFC nfc;

// Funktion überprüft, ob die gescannte Karte in der Speicherdatei vorhanden ist.
// Wenn ja, wird je nach Zustand loggedIn auf true oder false gesetzt.
void checkNFCTag()
{
    char *readValue = nfc.readTag();
    if (strcmp(readValue, "") != 0)
    {
        SerialMon.println(readValue);
        if (SPIFFSUtils::isRfidInSPIFFS(readValue))
        {
            loggedIn = !loggedIn;
            SerialMon.println(loggedIn ? "Auf" : "Zu");
            delay(2000);
        }
    }
    if (readValue != "")
    {
        delete[] readValue;
    }
}

void setup()
{
    SerialMon.begin(UART_BAUD);

    modem.powerOn();

    int arraySize;
    String *rfids = modem.getRfids(arraySize);

    if (rfids != nullptr)
    {
        SPIFFSUtils::saveRfidsToSPIFFS(rfids, arraySize);
        delete[] rfids;
    }

    nfc.init();
}

void loop()
{
    checkNFCTag();
}
