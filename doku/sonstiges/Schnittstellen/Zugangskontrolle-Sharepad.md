---
thema: Zugangskontrolle-Sharepad
typ: sonstiges
kategorie: allgemein
schlagworte: []
letzte_aktualisierung: 2026-05-14
---

---
thema: Zugangskontrolle-Sharepad
typ: sonstiges
kategorie: allgemein
schlagworte: []
letzte_aktualisierung: 2026-05-14
---

## Themenüberblick

Die Zugangskontrolle-Sharepad verwaltet den Zugang zu Ressourcen über eine Schnittstelle zum zentralen Server, wobei Kartenvalidierung und Reservierungsprüfung lokal erfolgen. Das System prüft zunächst die Karten-UID gegen verschlüsselte Sektordaten und gleicht diese mit lokalen Reservierungen und Whitelists ab. Bei fehlenden lokalen Reservierungen erfolgt eine optimierte Serverabfrage, die zunächst prüft, ob Buchungsänderungen vorliegen, bevor neue Reservierungsdaten heruntergeladen werden. Diese Vorgehensweise reduziert den Datenverkehr und verbessert die Systemeffizienz. Der Zugang wird nur gewährt, wenn eine gültige Reservierung oder Whitelist-Berechtigung vorliegt.

**Schlagworte:** Zugangskontrolle, Sharepad, Kartenvalidierung, Reservierungsprüfung, Serverabfrage, Whitelist, UID-Prüfung, Buchungsänderungen, Datenoptimierung, Ressourcenzugang

## Schnittstellendokumentation

## Schnittstellenübersicht

| # | Schnittstelle | Richtung | Integrationsart | Kurzbeschreibung |
|---|---------------|----------|-----------------|------------------|
| [1](#schnittstellendefinition-zugangskontrolle--sharepad-reservierungsabfrage) | Zugangskontrolle ↔ Sharepad | ZK → SP | REST API (Pull) | Abfrage von Reservierungen und Whitelists |
| [2](#schnittstellendefinition-zugangskontrolle--sharepad-änderungscheck) | Zugangskontrolle ↔ Sharepad | ZK → SP | HTTP (Pull) | Prüfung auf Reservierungsänderungen |
| [3](#schnittstellendefinition-zugangskontrolle--sharepad-live-events) | Zugangskontrolle → Sharepad | ZK → SP | REST API (Push) | Live-Übertragung von Zusperr-Events |
| [4](#schnittstellendefinition-zugangskontrolle--sharepad-kilometerübertragung) | Zugangskontrolle → Sharepad | ZK → SP | REST API (Push) | Übertragung von GPS-basierten Kilometerdaten |
| [5](#schnittstellendefinition-zugangskontrolle--sharepad-protokoll-upload) | Zugangskontrolle → Sharepad | ZK → SP | REST API (Push) | Periodischer Upload von ZK-Protokolldaten |

## Schnittstellendefinition: Zugangskontrolle ↔ Sharepad (Reservierungsabfrage)

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Hardware im Fahrzeug
- **System B:** Sharepad - Backend-System für Buchungsverwaltung
- **Integrationsrichtung:** ZK → Sharepad
- **Integrationsart:** REST API, HTTPS, Pull-Architektur

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Auto-Identifikation | ZK → SP | Eindeutige Auto-ID zur Identifikation (vorgesehen: IMEI des Modems, als QR-Code auf dem Gerät aufgedruckt, unabhängig von SIM-Karte) | ⚠️ Prozess der Zuordnung zu klären |
| 2 | Reservierungsdaten | SP → ZK | Aktuelle und zukünftige Reservierungen der nächsten 14 Tage (konfigurierbar; ggf. Split in "nächste 14 Tage" und "aktuell laufende Buchung") | ✅ klar |
| 3 | Reservierungsspezifische Whitelist | SP → ZK | NFC-Karten-UIDs pro Reservierung (ist Teil jeder Reservierung) | ✅ klar |
| 4 | Globale Whitelist | SP → ZK | Vereinsweite/sharepad-weite Whitelist (wird separat von den Reservierungen übertragen) | ⚠️ Abgrenzung zu klären |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Täglicher Vollsync | ZK (Timer) | Download aller Reservierungen | ✅ klar |
| 2 | Änderung erkannt (nach Änderungscheck) | ZK (nach S-02 Änderungscheck) | Download neuer Reservierungen inklusive Whitelist. Wird nur aufgerufen, wenn Änderungscheck "Änderung vorhanden" zurückmeldet | ✅ klar |
| 3 | Regelmäßige Prüfung während Reservierung | ZK (Timer) | Abfrage bei abgesperrtem Auto mit aktiver Reservierung | ✅ klar |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Auto-ID Zuordnungsprozess | IMEI als ID weitgehend beschlossen; Prozess der Zuordnung ZK ↔ Ressource ↔ Verein noch offen | Zuordnungsprozess definieren |
| 2 | Polling-Intervall | Viertelstündlich vs. 5-Minuten-Takt | Stromverbrauch vs. Aktualität abwägen |
| 3 | Whitelist-Abgrenzung | Nur Vereinsmitglieder, alle Sharepad-Nutzer, oder Nutzer mit Quernutzer-Vereinbarung? | Scope definieren |
| 4 | Split Buchungsendpunkte | Separater Endpunkt für "aktuelle Buchung jetzt" vs. "nächste 14 Tage"? | Tendenz ja, noch nicht final |
| 5 | Globale Whitelist Endpunkt | Separater API-Endpunkt oder zusammen mit Reservierungsdaten? | Tendenz getrennt |

### Annahmen
- Hardware unterstützt Schlafmodus, daher Pull-Architektur bevorzugt
- Reservierungen werden mit zugehörigen Whitelists übertragen
- Datenvolumen ist durch 500MB-Limit begrenzt

## Schnittstellendefinition: Zugangskontrolle ↔ Sharepad (Änderungscheck)

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Hardware im Fahrzeug
- **System B:** Sharepad - Backend-System für Buchungsverwaltung
- **Integrationsrichtung:** ZK → Sharepad
- **Integrationsart:** HTTP (unverschlüsselt), Pull-Architektur

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Auto-ID | ZK → SP | Identifikation des anfragenden Fahrzeugs | ✅ klar |
| 2 | Letzter Sync-Zeitstempel | ZK → SP | Zeitpunkt der letzten Reservierungsabfrage | ✅ klar |
| 3 | Änderungsstatus | SP → ZK | Ja/Nein-Antwort auf Änderungen | ✅ klar |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Regelmäßige Prüfung (abgesperrt + Reservierung) | ZK (Timer) | Änderungscheck alle 5-15 Minuten | ⚠️ Intervall zu definieren |
| 2 | Prüfung vor Reservierungsbeginn | ZK (Timer) | Änderungscheck 5 Minuten vor Termin | ✅ klar |
| 3 | Karte abgelehnt | ZK (User-Interaktion) | Sofortiger Änderungscheck | ✅ klar |
| 4 | Außerhalb von Buchungsfenstern | ZK (Timer) | Änderungscheck in längeren Intervallen (täglicher oder stündlicher Sync) | ⚠️ Intervall zu definieren |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Polling-Intervall | Optimierung zwischen Aktualität und Stromverbrauch | Finn: Stromverbrauchsmessungen |
| 2 | Datenvolumen | HTTP-Request-Größe ohne Nutzdaten | Messung durchführen |

### Annahmen
- HTTP ohne Verschlüsselung akzeptabel, da keine sensiblen Daten übertragen werden
- Zweistufiges Verfahren reduziert Datenvolumen erheblich
- Elegante Lösung zwischen vollwertigem Push und Pull

## Schnittstellendefinition: Zugangskontrolle → Sharepad (Live-Events)

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Hardware im Fahrzeug
- **System B:** Sharepad - Backend-System für Buchungsverwaltung
- **Integrationsrichtung:** ZK → Sharepad
- **Integrationsart:** REST API, HTTPS, Push-Architektur

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Auto-ID | ZK → SP | Identifikation des Fahrzeugs | ✅ klar |
| 2 | Karten-UID | ZK → SP | NFC-Karte die zum Zusperren verwendet wurde | ✅ klar |
| 3 | Zeitstempel | ZK → SP | Exakter Zeitpunkt des Zusperrens | ✅ klar |
| 4 | GPS-Koordinaten | ZK → SP | Standort beim Zusperren | ⚠️ Datenschutz zu klären |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Kartenzusperren | ZK (User-Interaktion) | Sofortige Live-Nachricht an Sharepad | ✅ klar |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | GPS-Datenschutz | Speicherung während vs. nach Buchungsende | Datenschutzkonzept klären |
| 2 | Zwischenzusperren | Behandlung von Zusperren während laufender Buchung | Interpretation im Backend |

### Annahmen
- Jedes Kartenzusperren wird protokolliert, auch während laufender Buchung
- Absperren per Karte ist ein Indiz, aber kein sicherer Hinweis auf das Buchungsende (Nutzer kann danach mit physischem Schlüssel weiterfahren). Sharepad nutzt es als Hinweis, trifft aber selbst die Entscheidung.
- Sharepad entscheidet über Reaktion (SMS, Verlängerung, Überziehungserkennung, Warnungen etc.)
- GPS-Koordinaten werden im Sharepad temporär je Buchung gespeichert. Nur die Koordinaten des letzten Absperrvorgangs vor Buchungsende werden dauerhaft als "Rückgabeort" gespeichert. Koordinaten während laufender Buchung werden nicht dauerhaft gespeichert.
- Datenschutzfragen zur GPS-Speicherung während Buchung sind noch mit dem Datenschutzbeauftragten zu klären.

## Schnittstellendefinition: Zugangskontrolle → Sharepad (Kilometerübertragung)

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Hardware im Fahrzeug
- **System B:** Sharepad - Backend-System für Buchungsverwaltung
- **Integrationsrichtung:** ZK → Sharepad
- **Integrationsart:** REST API, HTTPS, Push-Architektur

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Auto-ID | ZK → SP | Identifikation des Fahrzeugs | ✅ klar |
| 2 | Kilometerblöcke | ZK → SP | Von-bis Zeiträume mit gefahrenen Kilometern | ✅ klar |
| 3 | Zeitstempel | ZK → SP | Start- und Endzeit der Fahrtblöcke | ✅ klar |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Regelmäßige Übertragung | ZK (Timer) | Übertragung von Kilometerblöcken | 📋 Intervall zu definieren |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Übertragungsintervall | Täglich, wöchentlich oder bei Bedarf | Strategie definieren |
| 2 | Matching-Logik | Zuordnung zu Buchungen im Backend | Sharepad-seitige Implementierung |

### Annahmen
- Kilometererfassung unabhängig von Reservierungen und Auf-/Zusperren
- GPS-basierte Berechnung in der ZK
- Sharepad übernimmt Matching mit Buchungsdaten

## Schnittstellendefinition: Zugangskontrolle → Sharepad (Protokoll-Upload)

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Hardware im Fahrzeug
- **System B:** Sharepad - Backend-System für Buchungsverwaltung
- **Integrationsrichtung:** ZK → Sharepad
- **Integrationsart:** REST API, HTTPS, Push-Architektur

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Auto-ID | ZK → SP | Identifikation des Fahrzeugs | ✅ klar |
| 2 | Protokolldaten | ZK → SP | ZK-Aktivitätsprotokolle (Auf-/Zusperrvorgänge, Fehler, Systemstatus) | ⚠️ Format zu definieren |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Periodischer Upload | ZK (Timer) | Regelmäßige Übertragung der Protokolldaten | ⚠️ Intervall zu definieren |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Upload-Intervall | Wie oft werden Protokolle hochgeladen? | Strategie definieren |
| 2 | Protokollformat | Struktur und Inhalt der Protokolldaten | Format spezifizieren |
| 3 | Automatische Warnungen | Benachrichtigung bei fehlenden Protokollen oder Hardware-Fehlern (z.B. "NFC-Board nicht gefunden", ZK hat seit X Tagen keine Protokolle hochgeladen) | Alerting-Konzept erstellen |

### Annahmen
- Protokolle dienen der Überwachung und Fehlerdiagnose
- Sharepad kann automatische Warnungen generieren (z.B. E-Mail an zuständigen Administrator)
- Perspektivisch: Monitoring-Dashboard für ZK-Gesundheitsstatus

## Architekturprinzipien

- **Pull statt Push:** Die ZK pollt aktiv das Sharepad. Ein serverseitiger Push (z.B. über MQTT) ist wegen des Hardware-Schlafmodus und der Modem-Aufwachzeit technisch ungeeignet und wurde verworfen.
- **Zweiteilung leichtgewichtig/vollständig:** Der Änderungscheck ist eine minimale HTTP-Abfrage ohne Nutzdaten (kein TLS-Overhead). Nur bei positiver Antwort folgt der aufwändige HTTPS-Download der Reservierungsdaten.
- **Frontend bleibt "dumm":** Möglichst wenig Logik in der ZK. Alle Entscheidungen (Buchungsverwaltung, Abrechnung, Warnungen) liegen im Sharepad.
- **ZK-ID = IMEI:** Die ZK identifiziert sich anhand der IMEI des verbauten Modems. Diese ist als QR-Code auf dem Gerät aufgedruckt und unabhängig von der SIM-Karte.
- **Absperren per Karte = Indiz, kein Beweis:** Das ZK-Absperrsignal ist kein zuverlässiger Buchungsendpunkt (Nutzer kann mit physischem Schlüssel weiterfahren). Sharepad nutzt es als Hinweis, trifft aber selbst die Entscheidung.

## Glossar

| Begriff | Bedeutung |
|---------|----------|
| ZK | Zugangskontrolle - Arduino-basierte Hardware im Fahrzeug |
| Sharepad | Backend-System für Buchungs- und Nutzerverwaltung |
| NFC | Near Field Communication - Technologie für Kartenerkennung |
| UID | Unique Identifier - Eindeutige Kennung der NFC-Karten |
| Whitelist | Liste autorisierter NFC-Karten-UIDs |
| Pull-Architektur | ZK fragt aktiv Daten vom Server ab |
| Push-Architektur | ZK sendet Daten proaktiv an Server |
| REST API | Representational State Transfer - Webservice-Schnittstelle |
| HTTP/HTTPS | Hypertext Transfer Protocol (verschlüsselt/unverschlüsselt) |
| GPS | Global Positioning System - Satellitennavigation |
| Polling | Regelmäßige Abfrage von Daten |
| Schlafmodus | Energiesparender Zustand der Hardware |

---

## Quellen

| Datum | Thema | Transkript |
|-------|-------|------------|
| 2026-04-02 | Zugangskontrolle-Sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-02_Abstimmung-2026-04-02/transkript/transkript_2026-04-02.md) |
| 2026-04-13 | Zugangskontrolle-Sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-13_Abstimmung-2026-04-13/transkript/transkript_2026-04-13.md) |
