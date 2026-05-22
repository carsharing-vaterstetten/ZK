---
thema: Usecases ZK_sharepad
typ: sonstiges
kategorie: Usecases
schlagworte: []
letzte_aktualisierung: 2026-05-22
---

## Themenüberblick

Die Usecases für ZK_sharepad umfassen die zentralen Funktionen der Zugangskontrolle für Carsharing-Fahrzeuge. Der erste Usecase "Auto buchen" behandelt die Nutzeranmeldung und Auswahl der Mitgliedschaft (z.B. Marcel Meier mit privater und gewerblicher Zugehörigkeit), gefolgt von der Reservierungserstellung im Backend. Der zweite Usecase "Auto öffnen" beschreibt die NFC-Authentifizierung vor Ort, bei der die Karten-UID geprüft, entschlüsselt und gegen lokale Reservierungen abgeglichen wird. Weitere zentrale Usecases umfassen das Absperren von Fahrzeugen mit verschiedenen Berechtigungsebenen (terminspezifische vs. globale Whitelist), die Behandlung von Buchungsänderungen und Terminaktualisierungen sowie administrative Funktionen wie Kartenverwaltung, Nutzerregistrierung und Kilometererfassung. Das System arbeitet mit einer Pull-Architektur, bei der die Zugangskontrolle regelmäßig Änderungen beim Server abfragt, um aktuelle Buchungsdaten und Berechtigungen zu synchronisieren.

**Schlagworte:** Zugangskontrolle, NFC-Authentifizierung, Carsharing, Reservierungssystem, Whitelist-Management, Pull-Architektur, Buchungsänderungen, Nutzerregistrierung, Kilometererfassung, Backend-Integration

## Use Cases

| UC | Kurztitel | System | Kurzbeschreibung |
|-----|-----------|--------|------------------|
| [UC-01](#uc-01-auto-buchen) | Auto buchen | Sharepad | Eine Fahrzeugbuchung erstellen |
| [UC-02](#uc-02-auto-öffnen) | Auto öffnen | ZK / Übergreifend | Zugang zum reservierten Fahrzeug erlangen |
| [UC-03a](#uc-03a-auto-absperren-während-aktiver-reservierung) | Auto absperren (während aktiver Reservierung) | ZK | Das Fahrzeug während der Buchungszeit absperren |
| [UC-03b](#uc-03b-auto-absperren-nach-terminendeüberziehung) | Auto absperren (nach Terminende/Überziehung) | ZK | Das Fahrzeug nach Terminende absperren |
| [UC-03c](#uc-03c-überziehen--absperren-nach-terminende) | Überziehen / Absperren nach Terminende | ZK / Sharepad | Überziehungssituation erkennen und behandeln |
| [UC-03d](#uc-03d-vorzeitiges-absperren--übergabe) | Vorzeitiges Absperren / Übergabe | ZK / Sharepad | Auto vorzeitig zurückgeben und restliche Zeit freigeben |
| [UC-04](#uc-04-buchung-ändernstornieren) | Buchung ändern/stornieren | Sharepad / Übergreifend | Bestehende Buchungen anpassen oder löschen |
| [UC-05](#uc-05-terminaktualisierung-abfragen) | Terminaktualisierung abfragen | ZK / Übergreifend | Aktuelle Buchungsdaten vom Server abrufen |
| [UC-06](#uc-06-karte-sperrenlöschen) | Karte sperren/löschen | Sharepad / Übergreifend | Kartenzugang entziehen (z.B. bei verlorenem Führerschein) |
| [UC-07](#uc-07-auto-registrieren) | Auto registrieren | Sharepad / ZK | Neue ZK mit Fahrzeug und Verein verknüpfen |
| [UC-08](#uc-08-nutzer-registrieren) | Nutzer registrieren | Sharepad | Neuen Nutzer anlegen und mit Mitgliedschaften verknüpfen |
| [UC-09](#uc-09-chip-kodieren) | Chip kodieren | Externe Hardware / Sharepad | NFC-Karte mit Vereinsschlüssel kodieren |
| [UC-10](#uc-10-kilometererfassung) | Kilometererfassung | ZK / Sharepad | Gefahrene Kilometer automatisch erfassen und übertragen |
| [UC-11](#uc-11-firmware-update) | Firmware-Update | ZK / Sharepad | ZK-Software aktualisieren |
| [UC-12](#uc-12-protokolle-verwalten) | Protokolle verwalten | ZK / Sharepad | ZK-Aktivitäten protokollieren und überwachen |
| [UC-13](#uc-13-führerscheindaten-ändernfreigeben) | Führerscheindaten ändern/freigeben | Sharepad / Übergreifend | Führerscheindaten aktualisieren und für Fahrberechtigung freigeben |
| [UC-14](#uc-14-vereinswechsel-verwalten) | Vereinswechsel verwalten | Sharepad | Nutzer zwischen Vereinen wechseln oder mehrfach zuordnen |
| [UC-15](#uc-15-berechtigungen-verwalten) | Berechtigungen verwalten | Sharepad | Nutzerrollen und Berechtigungen vereinsspezifisch zuweisen |
| [UC-16](#uc-16-buchen-ohne-fahrberechtigung) | Buchen ohne Fahrberechtigung | Sharepad / ZK | Buchungen erstellen ohne Fahrzeugzugang |
| [UC-17](#uc-17-nutzer--mitglied-ändern) | Nutzer / Mitglied ändern | Sharepad | Stammdaten eines Nutzers oder Mitglieds ändern |
| [UC-18](#uc-18-nutzer--mitglied--ressource-stilllegen-und-löschen) | Nutzer / Mitglied / Ressource stilllegen und löschen | Sharepad | Lifecycle-Management für Nutzer, Mitglieder und Ressourcen |
| [UC-19](#uc-19-ressource-konfigurieren--ändern) | Ressource konfigurieren / ändern | Sharepad / ZK | Parameter einer Ressource ändern (z.B. Standort, Verhalten der ZK) |
| [UC-20](#uc-20-daten-exportieren--importieren) | Daten exportieren / importieren | Sharepad | Buchungsdaten exportieren und Kilometerdaten aus Drittsystemen importieren |
| [UC-21](#uc-21-auswertung) | Auswertung | Sharepad | Statistische Auswertungen über Buchungen, Nutzung, Kilometer |
| [UC-22](#uc-22-bestehende-karte-mit-user-verknüpfen) | Bestehende Karte mit User verknüpfen | Sharepad / Übergreifend | Bereits beklebten Führerschein bei neuem Verein registrieren |
| [UC-23](#uc-23-kartenverlust-vereinsweise-sperren) | Kartenverlust vereinsweise sperren | Sharepad | Verlorene oder gestohlene Karte vereinsspezifisch sperren |
| [UC-24](#uc-24-juristische-person-verwalten) | Juristische Person verwalten | Sharepad | Fahrberechtigte, Karten und Buchungsberechtigungen für juristische Personen verwalten |
| [UC-25](#uc-25-box-registrieren) | Box registrieren | Sharepad / ZK | Neue ZK-Box mit Seriennummer und QR-Code registrieren |

### UC-01: Auto buchen
- **System:** Sharepad
- **Akteur:** Nutzer
- **Ziel:** Eine Fahrzeugbuchung erstellen
- **Beschreibung:** Der Nutzer meldet sich im System an und ist einem oder mehreren Mitgliedern zugeordnet. Bei mehreren Mitgliedschaften (z.B. privat und gewerblich) wählt er die gewünschte Mitgliedsnummer für die Buchung aus. Das System erstellt eine Reservierung mit Nutzer, Mitglied, Zeitraum und Ressource.

### UC-02: Auto öffnen
- **System:** ZK / Übergreifend
- **Akteur:** Nutzer
- **Ziel:** Zugang zum reservierten Fahrzeug erlangen
- **Beschreibung:** Der Nutzer legt seine NFC-Karte auf den Reader. Die ZK prüft die UID und Verschlüsselung (Sektor 1 muss mit eigenem Schlüssel entschlüsselbar sein), dann die lokale Reservierungsliste. Bei fehlender Reservierung fragt die ZK beim Server nach aktuellen Terminen. Während des Downloads wird eine Visualisierung angezeigt (rotierende LEDs im Uhrzeigersinn in Orange). Bei erfolgreicher Prüfung wird das Auto geöffnet (2x grünes LED-Blinken + akustisches Signal). Bei Ablehnung: 2x rotes LED-Blinken. Bei fehlender Serververbindung erfolgt ein Fallback auf die globale Whitelist.
- **Sonderfall stornierte Buchung:** Eine Buchung kann zwischen letztem Sync und Aufsperr-Versuch storniert worden sein. Daher fragt die ZK vor jedem anstehenden Termin aktiv beim Server an, ob eine Buchungsänderung vorliegt (S-01, leichtgewichtig). Nur bei gemeldeter Änderung werden vollständige Buchungsdaten heruntergeladen (S-02).

### UC-03a: Auto absperren (während aktiver Reservierung)
- **System:** ZK
- **Akteur:** Nutzer mit gültiger Reservierung
- **Ziel:** Das Fahrzeug während der Buchungszeit absperren
- **Beschreibung:** Der Nutzer legt seine NFC-Karte auf den Reader. Die ZK prüft, ob eine aktive Reservierung vorliegt und die Karte in der terminspezifischen Whitelist steht. Nur berechtigte Karten können das Auto während der Reservierung absperren. Bei erfolgreichem Absperren sendet die ZK sofort eine Live-Nachricht an den Server (S-03) mit: Karten-UID, Uhrzeit, Datum, GPS-Koordinaten. Jedes Absperren per Karte während einer Buchung wird übermittelt. Das Absperren per Karte ist ein Indiz, aber kein sicherer Hinweis auf das Buchungsende (Nutzer kann danach mit Schlüssel weiterfahren).

### UC-03b: Auto absperren (nach Terminende/Überziehung)
- **System:** ZK
- **Akteur:** Nutzer mit globaler Berechtigung
- **Ziel:** Das Fahrzeug nach Terminende absperren
- **Beschreibung:** Liegt keine aktive Reservierung vor, kann jeder Nutzer mit Karte in der globalen Whitelist das Auto absperren. Dies verhindert, dass Unbefugte mit beliebigen Karten das Fahrzeug absperren können.

### UC-03c: Überziehen / Absperren nach Terminende
- **System:** ZK / Sharepad
- **Akteur:** Nutzer / Server
- **Ziel:** Überziehungssituation erkennen und behandeln
- **Beschreibung:** Aus ZK-Sicht meldet die ZK das Absperrereignis an den Server. Was der Server daraus macht (Abrechnung, Warnung, SMS etc.), liegt im Backend. Perspektivisch kann der Server eine SMS/Warnung senden, wenn kein Absperrsignal kurz vor oder nach dem Terminende eingeht.

### UC-03d: Vorzeitiges Absperren / Übergabe
- **System:** ZK / Sharepad
- **Akteur:** Nutzer
- **Ziel:** Auto vorzeitig zurückgeben und restliche Zeit freigeben
- **Beschreibung:** Ein Nutzer gibt das Auto früher zurück und gibt die restliche Zeit frei. Ein Dritter bucht das Auto nach. Fliegende Übergabe: Die ZK muss zum Aufsperren des neuen Nutzers aktualisierte Buchungsdaten kennen (Sync via S-01/S-02). Bei fliegender Übergabe ohne ZK-Aufsperren hat die ZK noch die alte Buchung. Regelmäßiges Polling löst dies innerhalb des definierten Zeitintervalls.

### UC-04: Buchung ändern/stornieren
- **System:** Sharepad / Übergreifend
- **Akteur:** Nutzer oder Administrator
- **Ziel:** Bestehende Buchungen anpassen oder löschen
- **Beschreibung:** Buchungen können gelöscht, verkürzt oder verschoben werden. Bei Änderungen müssen alle betroffenen ZK-Systeme über die Terminänderung informiert werden. Die ZK fragt vor Terminbeginn und bei Ablehnungen nach Änderungen beim Server ab. Polling-Strategie nach ZK-Zustand:
  - **Außerhalb von Buchungsfenstern:** Täglicher oder stündlicher Sync (Intervall offen).
  - **Ab ca. 5 Minuten vor Buchungsstart bis Auto aufgesperrt:** Engmaschiges Polling (möglichst kurze Wartezeit für Nutzer).
  - **Auto abgesperrt + aktive Buchung:** Polling im ca. Viertelstunden-Takt, um stornierte Buchungen zeitnah zu erkennen.
  - **Auto aufgesperrt:** Kein Polling nötig (Auto fährt).

### UC-05: Terminaktualisierung abfragen
- **System:** ZK / Übergreifend
- **Akteur:** ZK-System
- **Ziel:** Aktuelle Buchungsdaten vom Server abrufen
- **Beschreibung:** Die ZK fragt in zwei Stufen ab: Erst "gibt es Änderungen?", dann bei Bedarf die vollständigen Terminlisten. Dies erfolgt 5 Minuten vor Terminbeginn, bei Kartenablehnungen und regelmäßig während abgesperrter Zustände mit aktiven Terminen. Basierend auf historischen Daten werden bei stündlicher Synchronisation 90% der Buchungen erfasst, bei 5-minütlicher Synchronisation 98,7%. Die Termin-Whitelists werden über die Member-User-NFC-ID Kette berechnet: Zu einem Member werden alle User gesucht und zu jedem User die verknüpften NFC-IDs.

### UC-06: Karte sperren/löschen
- **System:** Sharepad / Übergreifend
- **Akteur:** Administrator
- **Ziel:** Kartenzugang entziehen (z.B. bei verlorenem Führerschein)
- **Beschreibung:** Der Administrator entfernt eine Karten-UID aus dem System. Die UID wird aus allen Termin-Whitelisten und der globalen Whitelist entfernt. Alle betroffenen ZK-Systeme werden über die Änderung informiert. Die UID bleibt zur Nachverfolgbarkeit in historischen Daten erhalten. Bei Doppelmitgliedschaften muss jeder betroffene Verein die Sperrung separat durchführen - es gibt keine automatische vereinsübergreifende Sperrung.

### UC-07: Auto registrieren
- **System:** Sharepad / ZK
- **Akteur:** Administrator
- **Ziel:** Neue ZK mit Fahrzeug und Verein verknüpfen
- **Beschreibung:** Eine vorkonfigurierte ZK wird mit einem Fahrzeug und einem Verein "verheiratet". Die eindeutige ZK-ID wird im System registriert und der entsprechenden Ressource zugeordnet.

### UC-08: Nutzer registrieren
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Neuen Nutzer anlegen und mit Mitgliedschaften verknüpfen
- **Beschreibung:** Ein Administrator legt einen neuen Nutzer an und verknüpft ihn mit einem oder mehreren Vereinen und Mitgliedschaften. Ein Nutzer kann bei mehreren Vereinen gleichzeitig Mitglied sein. Selbstregistrierung durch Nutzer ist nicht vorgesehen, da der Regelfall eine Führerscheinprüfung beim Verein erfordert.

### UC-09: Chip kodieren
- **System:** Externe Hardware / Sharepad
- **Akteur:** Administrator
- **Ziel:** NFC-Karte mit Vereinsschlüssel kodieren
- **Beschreibung:** Die UID der Karte wird mit dem Vereinsschlüssel in Sektor 1 verschlüsselt gespeichert. Die UID wird im System dem entsprechenden Nutzer zugeordnet und in die globale Whitelist aufgenommen.

### UC-10: Kilometererfassung
- **System:** ZK / Sharepad
- **Akteur:** ZK-System
- **Ziel:** Gefahrene Kilometer automatisch erfassen und übertragen
- **Beschreibung:** Die ZK erfasst GPS-basiert Kilometerdaten unabhängig von Buchungen und überträgt diese in Zeitblöcken an Sharepad. Das System matcht die Kilometerdaten nachträglich mit den Buchungsdaten für die Abrechnung.

### UC-11: Firmware-Update
- **System:** ZK / Sharepad
- **Akteur:** Administrator
- **Ziel:** ZK-Software aktualisieren
- **Beschreibung:** Firmware-Updates werden zentral verwaltet und an die ZK-Systeme verteilt. Das System überwacht Versionen und kann Updates gezielt oder automatisch durchführen.

### UC-12: Protokolle verwalten
- **System:** ZK / Sharepad
- **Akteur:** ZK-System/Administrator
- **Ziel:** ZK-Aktivitäten protokollieren und überwachen
- **Beschreibung:** Die ZK überträgt regelmäßig Protokolldaten an Sharepad. Das System überwacht die Protokollierung und kann Warnungen bei Ausfällen oder Fehlern generieren (z.B. fehlende Protokolle, Hardware-Probleme).

### UC-13: Führerscheindaten ändern/freigeben
- **System:** Sharepad / Übergreifend
- **Akteur:** Administrator
- **Ziel:** Führerscheindaten aktualisieren und für Fahrberechtigung freigeben
- **Beschreibung:** Bei Änderungen der Führerscheindaten (z.B. neue Adresse, Führerscheinerneuerung) erfasst der Verein die neuen Daten und gibt sie frei. Bei Nutzern mit mehreren Vereinsmitgliedschaften werden alle betroffenen Vereine über die Änderung informiert. Nur Vereine, die die aktuellen Daten freigegeben haben, dürfen den Nutzer in ihre Buchungs-Whitelists aufnehmen.

### UC-14: Vereinswechsel verwalten
- **System:** Sharepad
- **Akteur:** Administrator/Nutzer
- **Ziel:** Nutzer zwischen Vereinen wechseln oder mehrfach zuordnen
- **Beschreibung:** Ein Nutzer kann von einem Verein zu einem anderen wechseln oder bei mehreren Vereinen gleichzeitig Mitglied sein. Bei Vereinswechseln bleiben die Nutzerdaten für Abrechnungszwecke beim alten Verein erhalten. Der neue Verein kann die bestehenden Nutzerdaten einsehen und muss die Führerscheindaten erneut prüfen und freigeben.

### UC-15: Berechtigungen verwalten
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Nutzerrollen und Berechtigungen vereinsspezifisch zuweisen
- **Beschreibung:** Nutzer sind standardmäßig Anwender. Administrator- oder Supervisor-Rechte werden vereinsspezifisch durch Aufnahme in entsprechende Listen vergeben. Supervisor und Admin haben ähnliche Buchungsrechte, nur Admin hat zusätzlich Zugang zur Admin-Oberfläche. Vereinsangestellte ohne Mitgliedschaft werden über spezielle Orga-Mitgliedschaften verwaltet. Beim Entfernen aus allen Mitgliedschaften eines Vereins erlöschen automatisch die erweiterten Berechtigungen.

### UC-16: Buchen ohne Fahrberechtigung
- **System:** Sharepad / ZK
- **Akteur:** Nutzer ohne freigegebene Führerscheindaten
- **Ziel:** Buchungen erstellen ohne Fahrzeugzugang
- **Beschreibung:** Nutzer können Buchungen erstellen, auch wenn ihre Führerscheindaten nicht freigegeben sind. Sie können jedoch die gebuchten Fahrzeuge nicht öffnen oder schließen, da ihre UID nicht in den entsprechenden Whitelists steht. Dies ermöglicht z.B. Vereinsangestellten ohne Führerschein die Buchungsverwaltung.

### UC-17: Nutzer / Mitglied ändern
- **System:** Sharepad
- **Akteur:** Nutzer / Administrator
- **Ziel:** Stammdaten eines Nutzers oder Mitglieds ändern
- **Beschreibung:** Unterscheidung welche Felder der Nutzer selbst ändern darf und welche nur der Administrator. UID-Änderung (z.B. neuer Chip nach Führerscheinverlust) betrifft automatisch alle Termin-Whitelisten und wird über den Sync-Mechanismus (UC-05/UC-06) an die ZK-Systeme propagiert.

### UC-18: Nutzer / Mitglied / Ressource stilllegen und löschen
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Lifecycle-Management für Nutzer, Mitglieder und Ressourcen
- **Beschreibung:** Sofortiges Löschen ist nicht möglich, da abrechnungsrelevante Daten erhalten bleiben müssen. Prozess: Stilllegen, dann nach adäquater Zeit (Mitglieder: ca. 6-12 Monate nach Kündigung) tatsächliches Löschen. Bei Stilllegung werden die betroffenen UIDs aus aktiven Whitelists entfernt.

### UC-19: Ressource konfigurieren / ändern
- **System:** Sharepad / ZK
- **Akteur:** Administrator
- **Ziel:** Parameter einer Ressource ändern (z.B. Standort, Verhalten der ZK)
- **Beschreibung:** Konfiguration soll soweit wie möglich im Sharepad erfolgen. ZK-relevante Parameter sind noch zu definieren. Abrechnungsparameter (Stunden-/Kilometersätze) sind relevant, aber außerhalb der ZK-Schnittstelle. Stilllegen statt sofortigem Löschen wegen Abrechnungsrelevanz.

### UC-20: Daten exportieren / importieren
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Buchungsdaten exportieren und Kilometerdaten aus Drittsystemen importieren
- **Beschreibung:** Aktuell läuft der Kilometer-Ist-Zustand über Access (Eingabe) und einen Sharepad-Export. Zukünftig soll dies über die ZK-Schnittstelle automatisiert werden (UC-10). Details noch zu definieren.

### UC-21: Auswertung
- **System:** Sharepad
- **Akteur:** Administrator / Nutzer
- **Ziel:** Statistische Auswertungen über Buchungen, Nutzung, Kilometer
- **Beschreibung:** Für den aktuellen Scope bewusst nicht vorgesehen (zurückgestellt).

### UC-22: Bestehende Karte mit User verknüpfen
- **System:** Sharepad / Übergreifend
- **Akteur:** Administrator
- **Ziel:** Bereits beklebten Führerschein bei neuem Verein registrieren
- **Beschreibung:** Ein Nutzer wechselt zu einem neuen Verein oder wird zusätzlich bei einem zweiten Verein Mitglied und möchte seinen bereits mit NFC-Chip beklebten Führerschein weiternutzen. Der Administrator trägt die Nutzerdaten in die App ein, scannt den vorhandenen Chip und sendet die Daten an das Backend. Das System erstellt eine neue Verknüpfung zwischen der bestehenden NFC-ID und dem neuen User. Als Sicherheitsfeature wird zurückgemeldet, bei welchen anderen Vereinen diese ID bereits registriert ist. Bei unerwarteten Mehrfachregistrierungen kann eine Doppel-ID-Situation erkannt werden.

### UC-23: Kartenverlust vereinsweise sperren
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Verlorene oder gestohlene Karte vereinsspezifisch sperren
- **Beschreibung:** Bei Verlust oder Diebstahl einer NFC-Karte meldet das Mitglied dies dem Verein. Der Administrator kann die entsprechende UID über die User-Zuordnung finden und aus den Whitelists des eigenen Vereins entfernen. Bei Doppelmitgliedschaften muss jeder betroffene Verein separat informiert werden und die Sperrung eigenständig durchführen. Eine automatische vereinsübergreifende Sperrung erfolgt nicht, um die Vereinsautonomie zu wahren.

### UC-24: Juristische Person verwalten
- **System:** Sharepad
- **Akteur:** Administrator
- **Ziel:** Fahrberechtigte, Karten und Buchungsberechtigungen für juristische Personen verwalten
- **Beschreibung:** Juristische Personen (Firmen, Gemeinden) erhalten eine flexible Verwaltung über drei User-Kategorien: Karten-User (für physische Karten ohne Personenbezug), Fahrberechtigte-User (für jährliche Führerscheinlisten ohne Login) und Buchungs-User (für Login-Berechtigung ohne Fahrberechtigung). Ein Mitarbeiter kann in allen drei Kategorien gleichzeitig geführt werden. Die juristische Person übermittelt jährlich eine Liste aller Fahrberechtigten mit Bestätigung gültiger Führerscheine. Karten können am Kühlschrank hängen oder zentral verwaltet werden.

### UC-25: Box registrieren
- **System:** Sharepad / ZK
- **Akteur:** Administrator
- **Ziel:** Neue ZK-Box mit Seriennummer und QR-Code registrieren
- **Beschreibung:** ZK-Boxen werden mit vorprogrammierter Seriennummer geliefert und benötigen keine weitere Programmierung durch den Verein. Die Seriennummer ist als QR-Code außen auf der Box aufgedruckt, um das Scannen ohne Aufschrauben zu ermöglichen. Der Administrator scannt den QR-Code und ordnet die Box dem entsprechenden Fahrzeug und Verein zu.

## Glossar
| Begriff | Bedeutung |
|---------|----------|
| ZK | Zentralkontrolle - Hardware-Box im Fahrzeug für NFC-basierte Zugangssteuerung |
| NFC-Karte | Near Field Communication Karte mit eindeutiger UID für Fahrzeugzugang |
| UID | Unique Identifier - eindeutige Kennung der NFC-Karte |
| Whitelist | Liste berechtigter NFC-UIDs für Fahrzeugzugang |
| Termin-Whitelist | Buchungsspezifische Liste berechtigter UIDs für eine Reservierung |
| Globale Whitelist | Vereinsweite Liste aller aktiven NFC-UIDs |
| Member | Mitgliedschaft einer Person oder Organisation bei einem Verein |
| User | Benutzer im System, kann natürliche oder juristische Person sein |
| Login | Anmeldedaten für Zugang zum Buchungssystem |
| Supervisor | Benutzerrolle mit erweiterten Buchungsrechten, aber ohne Admin-Zugang |
| Sektor 1 | Verschlüsselter Bereich der NFC-Karte mit vereinsspezifischen Daten |
| Polling | Regelmäßige Abfrage der ZK beim Server nach Aktualisierungen |
| Fliegende Übergabe | Fahrzeugwechsel zwischen Nutzern ohne ZK-Kontakt |

## Quellen

| Datum | Thema | Transkript |
|-------|-------|------------|
| 2026-04-02 | Usecases ZK_sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-02_Abstimmung-2026-04-02/transkript/transkript_2026-04-02.md) |
| 2026-04-13 | Usecases ZK_sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-13_Abstimmung-2026-04-13/transkript/transkript_2026-04-13.md) |
| 2026-05-21 | Usecases ZK_sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-05-21_Abstimmung-2026-05-20/transkript/transkript_2026-05-21.md) |
