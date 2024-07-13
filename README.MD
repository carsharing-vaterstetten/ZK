# ZK

## Inhaltsverzeichnis

- [Übersicht](#übersicht)
- [Hardware](#hardware)
  - [Pinbelegung](#pinbelegung)
- [Software](#software)
  - [Abhängigkeiten](#abhängigkeiten)
- [Installation und Einrichtung](#installation-und-einrichtung)
- [Change Log](#change-log)

## Übersicht

Dieses Projekt implementiert ein RFID-Zugangskontrollsystem auf Basis eines ESP32-Boards. Es ermöglicht das Scannen von RFID-Tags und überprüft, ob diese in einer gespeicherten Liste von autorisierten Tags enthalten sind. Wenn ein autorisierter Tag erkannt wird, wird der Zugang gewährt oder verweigert.

## Hardware

- **ESP32-Board**: Das Haupt-Mikrocontroller-Board, das die Logik des Systems steuert.
- **RFID-Modul (Adafruit PN532)**: Ein NFC/RFID-Modul, das zum Scannen und Lesen von RFID-Tags verwendet wird.
- **SIM7000 Modem**: Ein GSM/LTE-Modem für die Kommunikation mit einem entfernten Server zur Aktualisierung der RFID-Liste.

### Pinbelegung

- Diese können in der `include/Config.h` Datei angepasst werden.

- **Modem Pins**:
  - `PIN_TX`: 27
  - `PIN_RX`: 26
  - `PWR_PIN`: 4

- **NFC Modul Pins**:
  - `NFC_MISO`: 18
  - `NFC_MOSI`: 19
  - `NFC_SCLK`: 23
  - `NFC_SS`: 5

- **LED Pins**:
  - `LED_PIN`: 12
  - `LED_COUNT`: 3

## Software

### Abhängigkeiten

Die folgenden Bibliotheken werden verwendet und sind in der `platformio.ini` Datei definiert:

- `TinyGSM`
- `ArduinoHttpClient`
- `ArduinoJson`
- `Adafruit PN532`
- `Adafruit NeoPixel`

## Installation und Einrichtung

1. **Hardware verbinden**: Verbinden Sie das ESP32-Board mit dem RFID-Modul und dem SIM7000-Modem gemäß der oben angegebenen Pinbelegung.
2. **Software einrichten**: Klonen Sie das Repository und öffnen Sie es in Visual Studio Code mit der PlatformIO-Erweiterung.
3. **Konfiguration anpassen**: Bearbeiten Sie die `include/Config.h` Datei, um die Serverinformationen und andere Konfigurationen anzupassen.
4. **Server einrichten**: Richten Sie einen Server ein, der die RFID-Liste hostet und die Liste über eine HTTPS-Schnittstelle aktualisieren kann.
5. **Projekt kompilieren und hochladen**: Kompilieren Sie das Projekt und laden Sie es auf das ESP32-Board hoch.

## Change Log

### [0.0.2]

#### Hinzugefügt

- `Adafruit NeoPixel` Bibliothek und Funktionen für die LED-Steuerung.
  - Nach dem Scannen eines RFID-Tags wird die LED grün oder rot beleuchtet.
