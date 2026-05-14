---
thema: Usecases ZK_sharepad
typ: sonstiges
kategorie: allgemein
schlagworte: []
letzte_aktualisierung: 2026-05-14
---

---
thema: Usecases ZK/sharepad
typ: sonstiges
kategorie: allgemein
schlagworte: []
letzte_aktualisierung: 2026-05-14
---

## Themenüberblick

Das ZK_sharepad System behandelt verschiedene Use Cases für die Fahrzeugbuchung und -verwaltung in Carsharing-Vereinen. Die Diskussion zeigt, dass bei stündlicher Synchronisation 90% der Buchungen erfasst werden, wobei kurzfristige Stornierungen und Neubuchungen nur einen geringen Anteil ausmachen. Die Use Cases 2 und 3 für das Auf- und Zusperren von Fahrzeugen wurden überarbeitet, wobei Sonderfälle wie fehlende Serververbindung durch Fallback auf eine globale Whitelist gelöst werden. Ein zentraler Punkt ist die Nutzeranlegung (Use Case 6), bei der entschieden wurde, dass neue Benutzer durch den Verein und nicht durch Selbstregistrierung angelegt werden. Besondere Aufmerksamkeit gilt dem Mehrverein-Szenario, wo Nutzer bei Vereinswechseln oder parallelen Mitgliedschaften entsprechende Freigabeprozesse durchlaufen müssen.

**Schlagworte:** Synchronisation, Buchungsdaten, Use Cases, Fahrzeugzugang, Nutzeranlegung, Vereinsverwaltung, Mehrverein-Mitgliedschaft, Freigabeprozess, Whitelist, Berechtigungskonzept

## Use Cases

## Use Cases

### UC-01: Auto buchen
- **System:** Sharepad
- **Akteur:** Nutzer
- **Ziel:** Eine Fahrzeugbuchung erstellen
- **Beschreibung:** Der Nutzer meldet sich im System an und wählt bei mehreren verfügbaren Mitgliedschaften die gewünschte aus (z.B. Marcel Meier kann zwischen privater Familie Meier und gewerblicher Dondenville wählen). Anschließend erstellt er eine Reservierung für ein bestimmtes Fahrzeug mit Zeitraum. Das System speichert die Buchung mit Zuordnung von Nutzer, Mitglied, Zeitraum und Ressource.

### UC-02: Auto öffnen
- **System:** ZK/Sharepad (Übergreifend)
- **Akteur:** Nutzer
- **Ziel:** Zugang zum reservierten Fahrzeug erhalten
- **Beschreibung:** Der Nutzer legt seine NFC-Karte auf den Reader. Die ZK prüft die UID und Verschlüsselung, dann die lokale Reservierungsliste. Bei fehlender Reservierung fragt die ZK beim Server nach aktuellen Buchungen. Während des Downloads wird eine Visualisierung (rotierende LEDs) angezeigt. Bei erfolgreicher Prüfung wird das Auto geöffnet (Klack-Geräusch, zweimal grün blinken), bei Ablehnung zweimal rot blinken. Bei fehlender Serververbindung erfolgt ein Fallback auf die globale Whitelist.

### UC-03a: Auto absperren (während aktiver Reservierung)
- **System:** ZK
- **Akteur:** Nutzer mit gültiger Reservierung
- **Ziel:** Fahrzeug während der Buchungszeit sicher verschließen
- **Beschreibung:** Während einer aktiven Reservierung kann nur ein Nutzer, dessen Karte in der Whitelist der aktuellen Reservierung steht, das Fahrzeug absperren. Die ZK prüft die Karten-ID gegen die terminspezifische Whitelist und sperrt bei Berechtigung das Fahrzeug ab.

### UC-03b: Auto absperren (nach Terminende/Überziehung)
- **System:** ZK
- **Akteur:** Nutzer mit gültiger Sharepad-Karte
- **Ziel:** Fahrzeug nach Terminende oder bei Überziehung absperren
- **Beschreibung:** Liegt keine aktive Reservierung vor, kann jeder Nutzer mit einer Karte aus der globalen Whitelist das Fahrzeug absperren. Die ZK protokolliert die Karten-ID und den Zeitpunkt für spätere Zuordnung und Abrechnungszwecke. Jedes Absperren per Karte wird an Sharepad übermittelt.

### UC-04: Buchungsänderung bei laufender Reservierung
- **System:** Sharepad/ZK (Übergreifend)
- **Akteur:** Nutzer/Administrator
- **Ziel:** Bestehende Buchung anpassen oder stornieren
- **Beschreibung:** Ein Nutzer ändert oder storniert seine Buchung, während die ZK noch die alte Reservierung gespeichert hat. Die ZK fragt regelmäßig nach Änderungen - bei stündlicher Synchronisation werden 90% der Buchungen erfasst, bei 5-minütiger Synchronisation 98,7%. Bei Änderungen lädt sie die aktualisierte Reservierungsliste herunter und passt ihre lokalen Daten entsprechend an.

### UC-05: Karte sperren/entfernen
- **System:** Sharepad/ZK (Übergreifend)
- **Akteur:** Administrator
- **Ziel:** Verlorene oder gestohlene Karte deaktivieren
- **Beschreibung:** Ein Administrator entfernt eine Karten-UID aus dem System (z.B. bei verlorenem Führerschein). Das System entfernt die UID aus allen Reservierungs-Whitelisten und der globalen Whitelist. Alle betroffenen ZK-Einheiten werden über die Änderung informiert und aktualisieren ihre lokalen Whitelisten beim nächsten Abfragezyklus.

### UC-06: Vorzeitige Rückgabe
- **System:** Sharepad/ZK (Übergreifend)
- **Akteur:** Nutzer
- **Ziel:** Fahrzeug früher als geplant zurückgeben und für andere freigeben
- **Beschreibung:** Ein Nutzer beendet seine Fahrt vorzeitig und gibt die verbleibende Buchungszeit frei. Ein anderer Nutzer bucht das Fahrzeug für den freigegebenen Zeitraum. Die ZK erkennt durch regelmäßige Abfragen die Buchungsänderung und gewährt dem neuen Nutzer Zugang, auch wenn lokal noch die alte Reservierung gespeichert war.

### UC-07: Auto registrieren
- **System:** Sharepad/ZK (Übergreifend)
- **Akteur:** Administrator
- **Ziel:** Neue ZK-Einheit mit Fahrzeug und Verein verknüpfen
- **Beschreibung:** Eine vorkonfigurierte ZK-Einheit wird in ein Fahrzeug eingebaut und muss mit der Sharepad-Software, dem spezifischen Fahrzeug und dem Verein verknüpft werden. Der Registrierungsprozess stellt die Verbindung zwischen Hardware-ID, Fahrzeugdaten und Vereinszugehörigkeit her.

### UC-08: Nutzer registrieren
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Neuen Nutzer anlegen und mit Mitgliedschaften verknüpfen
- **Beschreibung:** Ein Administrator legt einen neuen Nutzer an und ordnet ihn einem Mitglied zu. Der Nutzer erhält einen QR-Code, mit dem er sich bei beliebig vielen Vereinen und Mitgliedschaften anmelden kann. Administratoren können den Nutzer über den QR-Code mit Mitgliedschaften verknüpfen und Berechtigungen vergeben. Eine Selbstregistrierung durch Nutzer ist nicht vorgesehen.

### UC-09: Chip kodieren
- **System:** Sharepad (mit Hardware-Tool)
- **Akteur:** Administrator
- **Ziel:** NFC-Chip für Nutzer programmieren
- **Beschreibung:** Ein Administrator kodiert einen NFC-Chip mit der Nutzer-UID und schreibt diese verschlüsselt in Sektor 1. Der Chip wird anschließend auf den Führerschein aufgebracht und die UID im System mit dem Nutzer verknüpft. Dies ermöglicht die spätere Authentifizierung an der ZK.

### UC-10: Kilometererfassung übertragen
- **System:** ZK/Sharepad (Übergreifend)
- **Akteur:** ZK (automatisch)
- **Ziel:** Gefahrene Kilometer an Sharepad übermitteln
- **Beschreibung:** Die ZK erfasst kontinuierlich GPS-basierte Kilometerdaten und übermittelt diese in Zeitblöcken an Sharepad (z.B. "von Uhrzeit X bis Y wurden Z Kilometer gefahren"). Sharepad ordnet diese Daten den entsprechenden Buchungen zu. Die Kilometererfassung funktioniert unabhängig von Buchungen und Zugangskontrolle.

### UC-11: Firmware-Update
- **System:** Sharepad/ZK (Übergreifend)
- **Akteur:** Administrator
- **Ziel:** ZK-Software aktualisieren
- **Beschreibung:** Ein Administrator initiiert ein Firmware-Update für eine oder mehrere ZK-Einheiten. Das System verwaltet Firmware-Versionen, prüft Kompatibilität und führt das Update durch. Nach erfolgreichem Update wird die neue Version im System dokumentiert und die ZK-Funktionalität überprüft.

### UC-12: Protokolle auswerten
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** ZK-Aktivitäten überwachen und Probleme erkennen
- **Beschreibung:** Die ZK überträgt regelmäßig Protokolldaten an Sharepad. Administratoren können diese einsehen und das System generiert automatische Warnungen bei Anomalien (z.B. fehlende Protokolle, Hardware-Fehler). Bei kritischen Problemen werden E-Mail-Benachrichtigungen an die zuständigen Administratoren gesendet.

### UC-13: Führerscheindaten ändern und freigeben
- **System:** Sharepad
- **Akteur:** Administrator/Nutzer
- **Ziel:** Führerscheindaten aktualisieren und vereinsspezifisch freigeben
- **Beschreibung:** Bei Änderungen der Führerscheindaten (z.B. Umzug, Führerscheinerneuerung) werden die neuen Daten erfasst und müssen von jedem Verein, bei dem der Nutzer Mitglied ist, separat freigegeben werden. Nur Vereine, die die aktuellen Daten freigegeben haben, dürfen die entsprechende UID in ihre Whitelisten aufnehmen. Andere Vereine werden automatisch über Datenänderungen informiert.

### UC-14: Vereinswechsel verwalten
- **System:** Sharepad
- **Akteur:** Administrator/Nutzer
- **Ziel:** Nutzer zwischen Vereinen wechseln oder bei mehreren Vereinen führen
- **Beschreibung:** Ein Nutzer wechselt von einem Verein zu einem anderen oder ist zeitweise bei mehreren Vereinen Mitglied (z.B. bei Umzug für Abrechnungszwecke). Der neue Verein kann die bestehenden Nutzerdaten einsehen, prüfen und freigeben. Bei Vereinswechsel ohne Umzug werden die Daten automatisch übernommen und müssen nur bestätigt werden.

### UC-15: Berechtigungen verwalten
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Nutzerrollen und Berechtigungen vereinsspezifisch zuweisen
- **Beschreibung:** Nutzer sind standardmäßig Anwender. Administrator- oder Super-User-Rechte werden vereinsspezifisch durch Aufnahme in entsprechende Listen vergeben. Verliert ein Nutzer seine Mitgliedschaft in einem Verein, erlöschen automatisch seine erweiterten Berechtigungen. Vereinsangestellte ohne Mitgliedschaft werden über ein spezielles Orga-Mitglied des Vereins geführt.

### UC-16: Buchung ohne Fahrberechtigung
- **System:** Sharepad
- **Akteur:** Nutzer ohne Führerscheinfreigabe
- **Ziel:** Fahrzeugbuchung für andere erstellen
- **Beschreibung:** Ein Nutzer ohne freigegebene Führerscheindaten (z.B. Vereinsangestellter ohne Führerschein) kann Buchungen erstellen, aber keine Fahrzeuge öffnen oder schließen. Dies ermöglicht es Mitarbeitern, administrative Buchungsaufgaben zu übernehmen, ohne selbst fahrberechtigt zu sein.

## Offene Fragen

- Genaue Definition der Kulanzzeiten beim Auf- und Absperren
- Behandlung von GPS-Koordinaten beim Absperren (Datenschutz vs. Validierung)
- Berechtigungskonzept für Nutzer-zu-Mitglied-Zuordnungen
- Vereinsübergreifende vs. vereinsspezifische Whitelisten
- Lifecycle-Management für Mitglieder, Nutzer und Ressourcen

---

## Quellen

| Datum | Thema | Transkript |
|-------|-------|------------|
| 2026-04-02 | Usecases ZK/sharepad | [Transkript](../../../../.doku-arbeitsbereich/2026-04-02_Abstimmung-2026-04-02/transkript/transkript_2026-04-02.md) |
| 2026-04-13 | Usecases ZK_sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-13_Abstimmung-2026-04-13/transkript/transkript_2026-04-13.md) |
