# ZK

## Inhaltsverzeichnis

- [Übersicht](#übersicht)
- [Hardware](#hardware)
  - [Pinbelegung](#pinbelegung)
- [Software](#software)
  - [Abhängigkeiten](#abhängigkeiten)
- [Installation und Einrichtung](#installation-und-einrichtung)
- [LED Bedeutung](#led-bedeutung)
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
  - `POWER_PIN`: 25 (Notwendig für das T-PCIE-Board)

- **NFC Modul Pins**:
  - `NCF_SDA`: 21
  - `NCF_SCL`: 22
  - `NFC_MISO`: 18
  - `NFC_MOSI`: 19
  - `NFC_SCLK`: 23
  - `NFC_SS`: 5

- **LED Pins**:
  - `LED_PIN`: 12
  - `LED_COUNT`: 3

- **Schlüssel Pins**:
  - `OPEN_KEY`: 15
  - `CLOSE_KEY`: 14
  > An diese Pins werden nach dem Lesen eines RFID-Tags Signale gesendet, um den Zugang zu gewähren oder zu verweigern.

## Software

### Abhängigkeiten

Die folgenden Bibliotheken werden verwendet und sind in der `platformio.ini` Datei definiert:

- `TinyGSM`
- `ArduinoHttpClient`
- `ArduinoJson`
  - Um das Programm einfach zu testen, siehe Website [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/)
- `Adafruit PN532`
- `Adafruit NeoPixel`

## Installation und Einrichtung

1. **Hardware verbinden**: Verbinden Sie das ESP32-Board mit dem RFID-Modul und dem SIM7000-Modem gemäß der oben angegebenen Pinbelegung.
2. **Software einrichten**: Klonen Sie das Repository und öffnen Sie es in Visual Studio Code mit der PlatformIO-Erweiterung.
3. **Konfiguration anpassen**: Bearbeiten Sie die `include/Config.h` Datei, um die Serverinformationen und andere Konfigurationen anzupassen.
4. **Server einrichten**: Richten Sie einen Server ein, der die RFID-Liste hostet und die Liste über eine HTTPS-Schnittstelle aktualisieren kann.
5. **Projekt kompilieren und hochladen**: Kompilieren Sie das Projekt und laden Sie es auf das ESP32-Board hoch.

## LED Bedeutung

- **Weiß**: Das ESP32-Board wurde gerade gestartet und wird initialisiert.
- **Lila**: Es wird nach Firmware-Updates gesucht und diese werden ggf. installiert.
- **Orange**: Die Zugangskarten werden aktualisiert.
- **Blau**: Logs werden auf den Server hochgeladen.

## Change Log

### [0.4.3]

#### Verändert

- Falls der interne Speicher beim Hochfahren nicht benutzt werden kann wird dieser formatiert

### [0.4.2]

#### Verändert

- Es wird sichergestellt, dass der Firmware-Update-Prozess korrekt beendet wird, wenn alle Bytes gelesen wurden

### [0.4.1]

#### Verändert

- Überarbeitung der Erstellung des `HTTP-Client`-Objekts.
- Anpassung der NFC-Lese-Funktionen: Diese verwenden jetzt `String`-Objekte anstelle von `char-Arrays`.

### [0.4.0]

#### Hinzugefügt

- In der `Config.h`-Datei kann die Unterstützung für das `T-PCIE`-Board mit `#define TINY_GSM_T_PCIE` aktiviert werden.
- Möglichkeit, das `Adafruit PN532` über I2C zu betreiben:
  - Verwenden Sie `#define NFC_I2C`, `#define NFC_SDA`, `#define NFC_SCL`.

### [0.3.0]

#### Breaking Changes

- Die Einstellung zum Verbinden mit dem Server wurde von `Config.h` entfernt. Nun muss beim erste initialiserung des ESP32 folgende Zeichenkette in Serial Monitor eingegeben werden:
  - `apn="";gprsUser="";gprsPass="";GSM_PIN="";server="";port=;username="";password="";`

### [0.2.0]

#### Hinzugefügt

- Wichtige Prozesse (z. B. das Auf- und Zusperren des Autos oder der Neustart des ESP32) werden in einer Log-Datei gespeichert und einmal täglich auf den Server hochgeladen.

### [0.1.0]

#### Hinzugefügt

- Eine neue Firmware-Version kann vom Server heruntergeladen und installiert werden.
  - Über die Mac-Adresse wird die zugehörige Firmware-Version ermittelt.
- Neue Farben für die LED-Steuerung wurden hinzugefügt.
- Das ESP32 wird zur definierten Uhrzeit `targetTimeToRestartESP32` neu gestartet, um die RFID-Liste zu aktualisieren und neue Firmware-Versionen zu installieren.

### [0.0.5]

#### Bugfix

- Fix Pin-Modus für `OPEN_KEY` und `CLOSE_KEY`

#### Verändert

- Pinbelegung für NFC-Modul

### [0.0.4]

#### Bugfix

- Fehler behoben, bei der Abfrage der RFID-Liste vom Server war die Antwort manchmal leer.

### [0.0.3]

#### Hinzugefügt

- Aktuelle Firmware-Version wird jetzt in `Intern.h` definiert.
- Nach auf und zu Sperren werden an die definierten Pins (`OPEN_KEY` und `CLOSE_KEY`) Signale gesendet.

### [0.0.2]

#### Hinzugefügt

- `Adafruit NeoPixel` Bibliothek und Funktionen für die LED-Steuerung.
  - Nach dem Scannen eines RFID-Tags wird die LED grün oder rot beleuchtet.