# Changelog

## [1.1.2]

- Maßnahmen zum Energiesparen:
  - Modem und GPS werden nur verwendet, wenn sie tatsächlich gebraucht werden
  - Falls das Modem nicht benötigt wird, wird es in den Schlafmodus versetzt
- Fehlerbehebungen

## [1.1.1]

- Modem wird auf LTE und CAT-M festgesetzt → verbessert Up- und Download von 2 kbit/s auf etwa 10 kbit/s und verkürzt
  die Verbindungsaufbauzeit.

## [1.1.0]

- Interne Code-Refaktorisierungen
- Behebt, dass das Modem beim ersten Versuch keine GPRS-Verbindung herstellt
- GPS wird erneut aktiviert, wenn der erste Versuch fehlschlägt
- LED blinkt zweimal grün beim Login und zweimal rot beim Logout statt einmal lang
- Neue Konfigurationsoption, um GPS-Aufzeichnung zu deaktivieren, wenn niemand eingeloggt ist
- Upload-Geschwindigkeit wird jetzt korrekt berechnet
- Geschwindigkeitstest der Verbindung erfolgt jetzt in einer Anfrage statt in zwei
- Watchdog wird vom HttpClient gepingt → Geschwindigkeitstests nicht mehr nötig → kann mit
  `GIVE_CONNECTION_SPEED_ESTIMATE` deaktiviert werden
- Firmware-Update wird in einer einzigen Anfrage geprüft und heruntergeladen statt in zwei
- Mehr Logging, kleine Fehlerbehebungen

## [1.0.1]

- Use HTTP caching to check for updated RFID UIDs in one instead of two requests
- Store logged in RFID to persist them over system restarts

## [1.0.0]

> [!CAUTION]
> Diese Version vernichtet alle Kompatibilität mit vorherigen Versionen vom Backend, alter EEPROM Configs und altem
> Dateisystem. Hardware und Backend muss neu aufgesetzt werden.

Bisher gibt es [hier](https://github.com/Finnomator/ZK/issues/1) eine kleine Übersicht.

## [0.5.1]

### Verändert

- Es wurde ein zweiter timer für den Hardware-Watchdog hinzugefügt welcher bei dem OverTheAir (OTA) Update aktiv wird.
  Somit kann der Download auch mal länger als die Zeit für den normalen Timer dauern.
    - Sobald der Firmwarecheck feststellt das ein Update nötig ist wird das Intervall auf
      `HW_WATCHDOG_OTA_UPDATE_TIMEOUT` = 3600 ms (1 Stunde) gesetzt.
    - Unanhängig davon ob das Update erfogreich war oder nicht wird der Watchdogtimer anschießend wieder auf
      `HW_WATCHDOG_DEFAULT_TIMEOUT` gesetzt.

## [0.5.0]

### Verändert

- Ein Hardware-Watchdog wurde aktiviert, um den Arduino bei unerwartetem Verhalten automatisch neu zu starten.
    - Der Arduino setzt den Watchdog regelmäßig alle `HW_WATCHDOG_RESET_DELAY_MS` = 100ms zurück.
    - Der tatsächliche Timeout liegt bei `HW_WATCHDOG_DEFAULT_TIMEOUT` = 300s (5min) – wird der Watchdog innerhalb
      dieser Zeit nicht zurückgesetzt,
      erfolgt ein automatischer Reset

## [0.4.4]

### Verändert

- Uhrzeit für Reboot geändert auf 03:30 Uhr
- Pins auf neues Shield angepasst
- modem delay reduziert

## [0.4.3]

### Verändert

- Falls der interne Speicher beim Hochfahren nicht benutzt werden kann wird dieser formatiert

## [0.4.2]

### Verändert

- Es wird sichergestellt, dass der Firmware-Update-Prozess korrekt beendet wird, wenn alle Bytes gelesen wurden

## [0.4.1]

### Verändert

- Überarbeitung der Erstellung des `HTTP-Client`-Objekts.
- Anpassung der NFC-Lese-Funktionen: Diese verwenden jetzt `String`-Objekte anstelle von `char-Arrays`.

## [0.4.0]

### Hinzugefügt

- In der `Config.h`-Datei kann die Unterstützung für das `T-PCIE`-Board mit `#define TINY_GSM_T_PCIE` aktiviert werden.
- Möglichkeit, das `Adafruit PN532` über I2C zu betreiben:
    - Verwenden Sie `#define NFC_I2C`, `#define NFC_SDA`, `#define NFC_SCL`.

## [0.3.0]

### Breaking Changes

- Die Einstellung zum Verbinden mit dem Server wurde von `Config.h` entfernt. Nun muss beim erste initialiserung des
  ESP32 folgende Zeichenkette in Serial Monitor eingegeben werden:
    - `apn="";gprsUser="";gprsPass="";GSM_PIN="";server="";port=;username="";password="";`

## [0.2.0]

### Hinzugefügt

- Wichtige Prozesse (z. B. das Auf- und Zusperren des Autos oder der Neustart des ESP32) werden in einer Log-Datei
  gespeichert und einmal täglich auf den Server hochgeladen.

## [0.1.0]

### Hinzugefügt

- Eine neue Firmware-Version kann vom Server heruntergeladen und installiert werden.
    - Über die Mac-Adresse wird die zugehörige Firmware-Version ermittelt.
- Neue Farben für die LED-Steuerung wurden hinzugefügt.
- Das ESP32 wird zur definierten Uhrzeit `targetTimeToRestartESP32` neu gestartet, um die RFID-Liste zu aktualisieren
  und neue Firmware-Versionen zu installieren.

## [0.0.5]

### Bugfix

- Fix Pin-Modus für `OPEN_KEY` und `CLOSE_KEY`

### Verändert

- Pinbelegung für NFC-Modul

## [0.0.4]

### Bugfix

- Fehler behoben, bei der Abfrage der RFID-Liste vom Server war die Antwort manchmal leer.

## [0.0.3]

### Hinzugefügt

- Aktuelle Firmware-Version wird jetzt in `Intern.h` definiert.
- Nach auf und zu Sperren werden an die definierten Pins (`OPEN_KEY` und `CLOSE_KEY`) Signale gesendet.

## [0.0.2]

### Hinzugefügt

- `Adafruit NeoPixel` Bibliothek und Funktionen für die LED-Steuerung.
    - Nach dem Scannen eines RFID-Tags wird die LED grün oder rot beleuchtet.
