---
thema: Zugangskontrolle-Sharepad
typ: sonstiges
kategorie: allgemein
schlagworte: []
letzte_aktualisierung: 2026-05-14
---



## Themenüberblick

Das Zugangskontrolle-Sharepad System verwaltet den Zugang zu Ressourcen über eine Schnittstelle zwischen lokalen Zugangskontrollgeräten und einem zentralen Server. Bei einer Kartenprüfung wird zunächst die Karten-UID validiert und anschließend geprüft, ob eine lokale Reservierung vorliegt. Falls keine lokale Reservierung vorhanden ist, fragt das System den Server ab, ob Änderungen bei den Buchungen vorliegen, bevor eine vollständige Reservierungsanfrage gestellt wird. Diese Optimierung dient der Dateneinsparung, da nur bei tatsächlichen Änderungen die kompletten Buchungsdaten inklusive Whitelist heruntergeladen werden. Der Prozess umfasst mehrere Verzweigungspunkte, an denen jeweils erst auf Änderungen geprüft wird, bevor eine Serveranfrage für neue Reservierungen erfolgt.

**Schlagworte:** Zugangskontrolle, Sharepad, Reservierung, Server-Schnittstelle, Karten-UID, Whitelist, Buchungsänderungen, Polling, Dateneinsparung, Prozessoptimierung

## Schnittstellendokumentation

## Schnittstellendefinition: Zugangskontrolle ↔ Server

### Übersicht
- **System A:** Zugangskontrolle (lokales System)
- **System B:** Server (zentrales Buchungssystem)
- **Integrationsrichtung:** A → B (Anfragen) / B → A (Antworten)
- **Integrationsart:** ⚠️ zu klären (vermutlich API-basiert)

### Datenaustausch
| # | Bezeichnung | Richtung | Beschreibung | Status |
|---|-------------|----------|--------------|--------|
| 1 | Änderungsabfrage | A → B | Prüfung ob Buchungsänderungen für aktuelle Ressourcen vorliegen | ✅ klar |
| 2 | Änderungsstatus | B → A | Antwort ob Änderungen vorhanden sind (ja/nein) | ✅ klar |
| 3 | Buchungsdaten-Download | B → A | Übertragung aktueller Buchungen inklusive Whitelist | ✅ klar |
| 4 | Reservierungsanfrage | A → B | Anfrage nach spezifischen Reservierungen (Legacy-Funktion) | ⚠️ wird ersetzt |

### Auslöser und Ereignisse
| # | Ereignis | Auslösendes System | Reaktion | Status |
|---|----------|--------------------|----------|--------|
| 1 | Keine lokale Reservierung gefunden | Zugangskontrolle | Änderungsabfrage an Server statt direkter Reservierungsanfrage | ✅ klar |
| 2 | Karte nicht in lokaler Whitelist | Zugangskontrolle | Änderungsabfrage an Server vor Reservierungsanfrage | ✅ klar |
| 3 | Änderungen vorhanden | Server | Download der aktuellen Buchungen inklusive Whitelist | ✅ klar |
| 4 | Keine Änderungen vorhanden | Server | Kartenablehnung ohne weitere Serveranfrage | ✅ klar |

### Offene Punkte
| # | Thema | Beschreibung | To-do |
|---|-------|--------------|-------|
| 1 | Polling-Frequenz | Wie oft wird nach Änderungen gefragt? | 📋 Frequenz definieren |
| 2 | Technische Implementierung | Art der Schnittstelle (API, Protokoll) nicht spezifiziert | 📋 Technische Details klären |
| 3 | Prozessdiagramm-Update | Allgemeines MD/Mermaid Flow Diagramm muss angepasst werden | 📋 Diagramm aktualisieren |

### Annahmen
- Die Änderungsabfrage dient der Datenersparnis gegenüber direkten Reservierungsanfragen
- Bei fehlenden Änderungen wird davon ausgegangen, dass keine neue Buchung vorliegt
- Das System arbeitet mit lokaler Zwischenspeicherung von Buchungsdaten und Whitelist


---

## Quellen

| Datum | Thema | Transkript |
|-------|-------|------------|
| 2026-04-13 | Zugangskontrolle-Sharepad | [Transkript](../../../.doku-arbeitsbereich/2026-04-13_Abstimmung-2026-04-13/transkript/transkript_2026-04-13.md) |
