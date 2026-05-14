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

Das Zugangskontrolle-Sharepad System verwaltet den Zugang zu Ressourcen durch eine intelligente Kombination aus lokaler Kartenerkennung und serverbasierter Buchungsvalidierung. Das System prüft zunächst die Authentizität von RFID-Karten durch Vergleich der UID mit verschlüsselten Sektordaten und gleicht diese anschließend mit lokalen Reservierungsdaten sowie einer Whitelist ab. Um Datenübertragung zu optimieren, wurde der Prozess so angepasst, dass vor jeder Serveranfrage zunächst geprüft wird, ob Änderungen bei den Buchungen vorliegen, bevor neue Reservierungsdaten heruntergeladen werden. Die Zugangsentscheidung erfolgt auf Basis der validierten Kartendaten und aktuellen Buchungsinformationen, wobei bei fehlenden oder ungültigen Reservierungen der Zugang verweigert wird.

**Schlagworte:** Zugangskontrolle, RFID-Karten, Reservierungsvalidierung, Serveranfrage, Buchungsänderungen, Whitelist, UID-Prüfung, Datenoptimierung, Kartenauthentifizierung, Polling-Frequenz

## Schnittstellendokumentation

## Schnittstellendokumentation

## Schnittstellendefinition: Zugangskontrolle ↔ Sharepad

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Fahrzeugzugangskontrolle mit NFC-Reader und Modem
- **System B:** Sharepad - Backend-System für Fahrzeugbuchungen und -verwaltung
- **Integrationsrichtung:** bidirektional
- **Integrationsart:** REST-API über HTTP/HTTPS, Pull-Architektur mit Live-Updates

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Reservierungsänderung prüfen | ZK → Sharepad | HTTP-Anfrage ohne Nutzdaten: "Gab es Änderungen seit letztem Sync?" | ✅ klar |
| 2 | Reservierungen abrufen | ZK → Sharepad | HTTPS-Anfrage mit Auto-ID, erhält aktuelle Reservierungen mit Whitelisten (nur bei Änderungen) | ✅ klar |
| 3 | Globale Whitelist abrufen | ZK → Sharepad | Abruf der vereinsübergreifenden NFC-Karten-Whitelist | ✅ klar |
| 4 | Absperr-Event übermitteln | ZK → Sharepad | Live-Nachricht bei Kartenzusperren: Auto-ID, Karten-UID, Zeitstempel, GPS-Koordinaten | ✅ klar |
| 5 | Auto-Identifikation | ZK → Sharepad | Eindeutige Auto-ID für alle Anfragen (vereinsübergreifend) | ⚠️ zu klären |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | 5 Minuten vor Terminbeginn | ZK (Timer) | Reservierungsänderung prüfen | ✅ klar |
| 2 | Während aktiver Reservierung (Auto zugesperrt) | ZK (Timer) | Regelmäßige Änderungsprüfung (5-15 Min Intervall) | ⚠️ zu klären |
| 3 | Karte wird abgelehnt (keine lokale Reservierung) | ZK (NFC-Event) | Änderungsprüfung → bei Änderung: Reservierungsabfrage → bei keiner Änderung: Ablehnung | ✅ klar |
| 4 | Karte nicht in Whitelist der Reservierung | ZK (NFC-Event) | Änderungsprüfung → bei Änderung: Reservierungsabfrage → bei keiner Änderung: Ablehnung | ✅ klar |
| 5 | Auto wird mit Karte zugesperrt | ZK (NFC-Event) | Live-Übertragung an Sharepad | ✅ klar |
| 6 | Whitelist-Änderung | Sharepad | Wird bei nächster Reservierungsabfrage übertragen | ✅ klar |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Auto-ID Format | Welche eindeutige ID verwendet die ZK zur Selbstidentifikation? (UID vs. andere) | Definition erforderlich |
| 2 | Polling-Intervalle | Optimale Abstände für Änderungsprüfungen (Stromverbrauch vs. Aktualität) | Finn: Stromverbrauchsmessungen |
| 3 | Modem-Modi | Stromverbrauch verschiedener Verbindungszustände und Aufweckzeiten | Finn: Technische Messungen |
| 4 | GPS-Datenschutz | Speicherdauer und -berechtigung für GPS-Koordinaten beim Absperren | Datenschutzprüfung |
| 5 | Häufigkeit kurzfristiger Buchungsänderungen | Wie oft werden Termine kurzfristig geändert/storniert? | Robert: Datenanalyse |
| 6 | Prozessdiagramm-Anpassung | Bestehende Mermaid-Diagramme müssen an zweistufige Abfrage angepasst werden | Dokumentation aktualisieren |

### Annahmen
- Push-Architektur wurde verworfen wegen Hardware-Schlafmodus-Problematik
- Zweistufiges Abfrageverfahren reduziert Datenvolumen erheblich durch Vorab-Änderungsprüfung
- GPS-Koordinaten werden nur bei endgültiger Rückgabe dauerhaft gespeichert
- Kartenzusperren während Buchung ist möglich, aber nicht automatisch Buchungsende
- ZK behält minimale Intelligenz, Entscheidungslogik liegt im Backend
- Bei jeder Kartenablehnung wird erst geprüft, ob Änderungen vorliegen, bevor vollständige Reservierungsdaten abgerufen werden

## Schnittstellendefinition: Zugangskontrolle ↔ Sharepad

### Übersicht
- **System A:** Zugangskontrolle (ZK) - Arduino-basierte Fahrzeugzugangskontrolle mit NFC-Reader und Modem
- **System B:** Sharepad - Backend-System für Fahrzeugbuchungen und -verwaltung
- **Integrationsrichtung:** bidirektional
- **Integrationsart:** REST-API über HTTP/HTTPS, Pull-Architektur mit Live-Updates

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Reservierungsänderung prüfen | ZK → Sharepad | HTTP-Anfrage ohne Nutzdaten: "Gab es Änderungen seit letztem Sync?" | ✅ klar |
| 2 | Reservierungen abrufen | ZK → Sharepad | HTTPS-Anfrage mit Auto-ID, erhält aktuelle Reservierungen mit Whitelisten | ✅ klar |
| 3 | Globale Whitelist abrufen | ZK → Sharepad | Abruf der vereinsübergreifenden NFC-Karten-Whitelist | ✅ klar |
| 4 | Absperr-Event übermitteln | ZK → Sharepad | Live-Nachricht bei Kartenzusperren: Auto-ID, Karten-UID, Zeitstempel, GPS-Koordinaten | ✅ klar |
| 5 | Auto-Identifikation | ZK → Sharepad | Eindeutige Auto-ID für alle Anfragen (vereinsübergreifend) | ⚠️ zu klären |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | 5 Minuten vor Terminbeginn | ZK (Timer) | Reservierungsänderung prüfen | ✅ klar |
| 2 | Während aktiver Reservierung (Auto zugesperrt) | ZK (Timer) | Regelmäßige Änderungsprüfung (5-15 Min Intervall) | ⚠️ zu klären |
| 3 | Karte wird abgelehnt | ZK (NFC-Event) | Sofortige Reservierungsabfrage | ✅ klar |
| 4 | Auto wird mit Karte zugesperrt | ZK (NFC-Event) | Live-Übertragung an Sharepad | ✅ klar |
| 5 | Whitelist-Änderung | Sharepad | Wird bei nächster Reservierungsabfrage übertragen | ✅ klar |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Auto-ID Format | Welche eindeutige ID verwendet die ZK zur Selbstidentifikation? (UID vs. andere) | Definition erforderlich |
| 2 | Polling-Intervalle | Optimale Abstände für Änderungsprüfungen (Stromverbrauch vs. Aktualität) | Finn: Stromverbrauchsmessungen |
| 3 | Modem-Modi | Stromverbrauch verschiedener Verbindungszustände und Aufweckzeiten | Finn: Technische Messungen |
| 4 | GPS-Datenschutz | Speicherdauer und -berechtigung für GPS-Koordinaten beim Absperren | Datenschutzprüfung |
| 5 | Häufigkeit kurzfristiger Buchungsänderungen | Wie oft werden Termine kurzfristig geändert/storniert? | Robert: Datenanalyse |

### Annahmen
- Push-Architektur wurde verworfen wegen Hardware-Schlafmodus-Problematik
- Zweistufiges Abfrageverfahren reduziert Datenvolumen erheblich
- GPS-Koordinaten werden nur bei endgültiger Rückgabe dauerhaft gespeichert
- Kartenzusperren während Buchung ist möglich, aber nicht automatisch Buchungsende
- ZK behält minimale Intelligenz, Entscheidungslogik liegt im Backend

---

## Quellen

| Datum | Thema | Transkript |
|-------|-------|------------|
| 2026-04-02 | Zugangskontrolle-Sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-02_Abstimmung-2026-04-02/transkript/transkript_2026-04-02.md) |
| 2026-04-13 | Zugangskontrolle-Sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-13_Abstimmung-2026-04-13/transkript/transkript_2026-04-13.md) |
