# ZK

Dieses Projekt implementiert ein RFID-Zugangskontrollsystem auf Basis eines LILYGO T-SIM7000G-Boards. Es ermöglicht das
Scannen von RFID-Tags und überprüft, ob diese in einer gespeicherten Liste von autorisierten Tags enthalten sind. Wenn
ein autorisierter Tag erkannt wird, wird der Zugang gewährt oder verweigert.

## Hardware

- [LILYGO T-SIM7000G](https://lilygo.cc/products/t-sim7000g)
- [PN532 NFC RFID module](https://www.elechouse.com/product/pn532-nfc-rfid-module-v4/)

### Pinbelegung

Die Pinbelegung kann in [`include/Config.h`](include/Config.h) angepasst werden.

## Firmware

### Installation und Einrichtung

1. **Hardware verbinden**: Verbinden Sie das LILYGO SIM7000G mit dem RFID-Modul gemäß der gewählten Pinbelegung und
   schließen Sie die Antennen an.
2. **Software einrichten**: Klonen Sie das Repository und öffnen Sie es in Visual Studio Code mit der
   PlatformIO-Erweiterung.
3. **Konfiguration anpassen**: Bearbeiten Sie die `include/Config.h` Datei
4. **Server einrichten**: Richten Sie einen Server ein, der die RFID-Liste hostet und die Liste über eine
   HTTPS-Schnittstelle aktualisieren kann.
5. **Projekt kompilieren und hochladen**: Kompilieren Sie das Projekt und laden Sie es auf das ESP32-Board hoch.

### LED Bedeutung

| LED Farbe | Bedeutung                                                                |
|-----------|--------------------------------------------------------------------------|
| Weiß      | Das ESP32-Board wurde gerade gestartet und wird initialisiert.           |
| Lila      | Es wird nach Firmware-Updates gesucht und diese werden ggf. installiert. |
| Orange    | Die Zugangskarten werden aktualisiert.                                   |
| Blau      | Logs werden auf den Server hochgeladen.                                  |

## Referenzen

| Resource              | Links                                                                                                                                                                                                                                                                        |
|-----------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Changelog             | [Changelog](CHANGELOG.md)                                                                                                                                                                                                                                                    |
| LILYGO SIM7000G       | [Hersteller Website](https://lilygo.cc/products/t-sim7000g)<br/>https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/<br/>https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G<br/>[LILYGO SIM7000G Schematic](blob:https://github.com/2dfb33f5-62f2-4441-8afc-19b310adb733) |
| PN532 NFC RFID module | [User Guide](https://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf)                                                                                                                                                                      |
