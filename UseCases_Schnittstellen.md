# Use Case Zusammenfassung – Zugangskontrolle (ZK) & Sharepad
**Gesprächsprotokoll vom 02.04.2026 – Marcel & Robert**

---

## Rollen & Begriffe

| Begriff | Beschreibung |
|---|---|
| **ZK (Zugangskontrolle)** | Das im Auto verbaute Gerät (Arduino-basiert) – das „Frontend". Zuständig für das Auf- und Absperren der Ressource. Entwicklung: Marcel. |
| **Sharepad** | Das Backend-System. Verwaltet Buchungen, Mitglieder, Nutzer, Vereine, Karten, Zugänge und weitere Stammdaten. Stellt eine JSON-REST-API bereit. Entwicklung: Robert. |
| **Nutzer** | Eine natürliche Person, die das System nutzt (fährt). Hat Login-Daten und kann 1–N UIDs (NFC-Chip-Nummern) besitzen. |
| **Mitglied** | Abrechnungseinheit; Verbund von Nutzern, die sich ein Abrechnungskonto teilen. Ein Mitglied gehört zu einem Verein. Ein Nutzer kann mehreren Mitgliedern zugeordnet sein. |
| **Verein** | Organisatorische Einheit im Sharepad. Aus ZK-Sicht weitgehend nicht relevant. |
| **Admin** | Rolle, die das System administriert. |
| **UID** | Eindeutige Nummer eines NFC-Chips (z. B. auf dem Führerschein aufgeklebt). Ein Nutzer kann 1–N UIDs haben. |
| **Ressource** | Ein buchbares Objekt, z. B. ein Auto. |
| **Whitelist (Termin)** | Liste der UIDs, die für eine bestimmte Reservierung das Auf-/Zusperren dürfen. |
| **Whitelist (global)** | Systemweite Liste aller aktiven UIDs. Wird durch das Sharepad bereitgestellt. Inhalte (vereinsspezifisch vs. sharepad-weit) werden noch festgelegt. |

---

## Übersicht Use Cases & Schnittstellen

### Use Cases

| ID | Bezeichnung | Definitionsgrad |
|---|---|:---:|
| [UC-01](#1-auto-buchen-uc-01) | Auto buchen | + |
| [UC-02](#2-auto-aufsperren-uc-02) | Auto aufsperren | + |
| [UC-03](#3-auto-absperren-uc-03) | Auto absperren | + |
| [UC-04](#4-terminänderung--buchungsänderung-uc-04) | Terminänderung / Buchungsänderung | o |
| [UC-05](#5-karte-sperren--uid-löschen-uc-05) | Karte sperren / UID löschen | o |
| [UC-06](#6-ressource-registrieren-uc-06) | Ressource registrieren | - |
| [UC-07](#7-ressource-konfigurieren--ändern-uc-07) | Ressource konfigurieren / ändern | - |
| [UC-08](#8-firmware-update-der-zk-uc-08) | Firmware-Update der ZK | -- |
| [UC-09](#9-karte-anlegen--karte-kodieren-uc-09) | Karte anlegen / Karte kodieren | - |
| [UC-10](#10-nutzer-anlegen--nutzer-einem-mitglied-zuordnen-uc-10) | Nutzer anlegen / einem Mitglied zuordnen | o |
| [UC-11](#11-nutzer--mitglied-ändern-uc-11) | Nutzer / Mitglied ändern | o |
| [UC-12](#12-nutzer--mitglied--ressource-stilllegen-und-löschen-uc-12) | Nutzer / Mitglied / Ressource stilllegen & löschen | - |
| [UC-13](#13-kilometererfassung-und--übermittlung-uc-13) | Kilometererfassung und -übermittlung | - |
| [UC-14](#14-zk-protokolle-hochladen-sichten-und-überwachen-uc-14) | ZK-Protokolle hochladen, sichten & überwachen | - |
| [UC-15](#15-berechtigungen-vergeben-uc-15) | Berechtigungen vergeben | -- |
| [UC-16](#16-daten-exportieren--importieren-uc-16) | Daten exportieren / importieren | - |
| [UC-17](#17-auswertung-uc-17) | Auswertung | o |

### Schnittstellen

| ID | Bezeichnung | Definitionsgrad |
|---|---|:---:|
| [S-01](#s-01--änderungsabfrage-leichtgewichtig--gibt-es-eine-änderung) | Änderungsabfrage (leichtgewichtig) | + |
| [S-02](#s-02--buchungen--termine-abrufen-https-vollständige-daten) | Buchungen / Termine abrufen (vollständig) | + |
| [S-03](#s-03--absperrdaten-live-übermitteln) | Absperrdaten live übermitteln | o |
| [S-04](#s-04--kilometer-übertragung-noch-nicht-vollständig-definiert) | Kilometer-Übertragung | -- |

**Legende:** `++` sehr präzise · `+` präzise · `o` okay · `-` ungenau · `--` sehr ungenau

---

## Use Cases

### 1. Auto buchen (UC-01)
**Kurzbeschreibung:** Ein Nutzer reserviert eine Ressource (Auto) über das Sharepad-Frontend.

- Der Nutzer meldet sich an und wählt ggf. das Mitglied, über das die Buchung laufen soll (z. B. privat oder gewerblich).
- Die Buchung wird mit folgenden Daten abgelegt: Nutzer, Mitglied, Ressource, Start- und Endzeit.
- Dieser Use Case betrifft ausschließlich das Backend/Sharepad. Die ZK ist nicht involviert.

**ZK-Schnittstellen:** keine

---

### 2. Auto aufsperren (UC-02)

**Kurzbeschreibung:** Ein Nutzer möchte eine reservierte Ressource in Betrieb nehmen und legt seine NFC-Karte an die ZK.

#### Ablauf (Normalfall):
1. Nutzer legt NFC-Karte auf den Reader.
2. ZK liest die UID und prüft zunächst, ob die UID im verschlüsselten Sektor 1 der Karte übereinstimmt und mit dem eigenen Schlüssel entschlüsselbar ist.
3. ZK prüft lokal: Gibt es eine aktuelle Reservierung?
4. Wenn Reservierung vorhanden: Steht die UID in der Termin-Whitelist?
   - Ja → Auto wird aufgesperrt (akustisch + 2× grünes LED-Blinken).
   - Nein → Karte wird abgelehnt (2× rotes LED-Blinken).
5. Wenn keine Reservierung lokal vorhanden: ZK fragt Sharepad an (Schnittstelle S-02), ob es eine aktuelle Buchung gibt. Wenn gefunden → Prüfung wie oben.

#### Sonderfall: Kein Netz / kein Server erreichbar
- Wenn weder Reservierung lokal noch via Server abrufbar: Fallback auf globale Whitelist.
- Steht die UID in der globalen Whitelist → weiterer Schritt (noch zu definieren).
- Steht sie nicht in der globalen Whitelist → Ablehnung.

#### Sonderfall: Stornierte Buchung
- Eine Buchung kann zwischen dem letzten lokalen Sync und dem Aufsperr-Versuch storniert worden sein.
- Daher: Vor jedem anstehenden Termin fragt die ZK aktiv beim Sharepad an, ob eine Buchungsänderung vorliegt (Schnittstelle S-01, leichtgewichtig).
- Nur wenn eine Änderung gemeldet wird, lädt die ZK die vollständigen Buchungsdaten herunter (Schnittstelle S-02).

#### Visualisierung während Wartezeit:
- Während die ZK Daten lädt (Serveranfrage läuft): Anzeige über LEDs (z. B. rotierender Lauf im Uhrzeigersinn in Orange), damit der Nutzer Feedback erhält und nicht mehrfach die Karte auflegt.

**ZK-Schnittstellen:** S-01 (Änderungsabfrage), S-02 (Buchungen/Termine abrufen)

---

### 3. Auto absperren (UC-03)

**Kurzbeschreibung:** Ein Nutzer sperrt das Auto mit seiner NFC-Karte ab.

#### 3a. Absperren während aktiver Buchung
- Nur UIDs aus der Termin-Whitelist der aktiven Reservierung dürfen absperren.
- Bei erfolgreichem Absperren: ZK sendet sofort eine Live-Nachricht an Sharepad (Schnittstelle S-03) mit: Karten-UID, Uhrzeit, Datum, GPS-Koordinaten des Absperr-Vorgangs.
- Das Absperren per Karte ist ein Indiz, aber kein sicherer Hinweis auf das Buchungsende (Nutzer kann danach mit Schlüssel weiterfahren).
- Jedes Absperren per Karte während einer Buchung wird übermittelt.

#### 3b. Absperren nach Ende der Buchung (Auto steht offen, kein aktiver Termin)
- Nur UIDs aus der globalen Whitelist dürfen absperren.
- Kein bekannter aktiver Termin mehr → Absperren durch jeden globalen Whitelist-Eintrag erlaubt.

#### 3c. Überziehen / Absperren nach Terminende
- Aus ZK-Sicht: Die ZK meldet das Absperrereignis an Sharepad. Was Sharepad daraus macht (Abrechnung, Warnung, SMS etc.), liegt im Backend.
- Perspektivisch: Sharepad kann eine SMS/Warnung senden, wenn kein Absperrsignal kurz vor oder nach dem Terminende eingeht.

#### 3d. Vorzeitiges Absperren / Übergabe
- Nutzer gibt das Auto früher zurück und gibt die restliche Zeit frei.
- Dritter bucht das Auto nach. Fliegende Übergabe: ZK muss zum Aufsperren des neuen Nutzers aktualisierte Buchungsdaten kennen (→ Sync via S-01/S-02).
- Problem: Bei fliegender Übergabe ohne ZK-Aufsperren hat die ZK noch die alte Buchung. Regelmäßiges Polling löst dies innerhalb des definierten Zeitintervalls.

**ZK-Schnittstellen:** S-01 (Änderungsabfrage), S-02 (Buchungen abrufen), S-03 (Absperrdaten live senden)

---

### 4. Terminänderung / Buchungsänderung (UC-04)

**Kurzbeschreibung:** Eine bestehende Buchung wird geändert (verlängert, verkürzt, storniert).

- Die ZK hat diesen Fall nicht selbst in der Hand. Sie reagiert über ihren Polling-Mechanismus.
- Zyklen der ZK (Polling-Strategie, noch zu definieren):
  - **Außerhalb von Buchungsfenstern:** Täglicher oder stündlicher Sync (Intervall offen).
  - **Ab ca. 5 Minuten vor Buchungsstart bis Auto aufgesperrt:** Engmaschiges Polling (Ziel: möglichst kurze Wartezeit für Nutzer beim Aufsperren). Intervall noch zu definieren – Robert prüft historische Daten über kurzfristige Stornierungen und Neubuchungen.
  - **Auto abgesperrt + aktive Buchung:** Polling im Viertelstunden-Takt oder ähnlichem (noch festzulegen), um stornierte Buchungen zeitnah zu erkennen.
  - **Auto aufgesperrt:** Kein Polling nötig (Auto fährt).

**ZK-Schnittstellen:** S-01 (Änderungsabfrage), S-02 (Buchungen abrufen)

---

### 5. Karte sperren / UID löschen (UC-05)

**Kurzbeschreibung:** Ein Administrator entfernt eine NFC-Karten-UID aus dem System (z. B. bei Verlust des Führerscheins).

- Im Sharepad: Die UID wird aus allen zukünftigen Termin-Whitelists und der globalen Whitelist entfernt.
- Die UID selbst wird nicht aus der UID-Verwaltungsdatenbank gelöscht (Nachverfolgbarkeit historischer Protokolleinträge).
- Alle Ressourcen, bei denen Buchungen dieser UID vorliegen, erhalten beim nächsten Sync aktualisierte Whitelisten (→ Polling UC-04 deckt dies ab).
- Aus ZK-Sicht: Ergibt sich aus der regulären Buchungs-/Whitelist-Aktualisierung.

**ZK-Schnittstellen:** S-01 (Änderungsabfrage), S-02 (Buchungen/Whitelist abrufen)

---

### 6. Ressource registrieren (UC-06)

**Kurzbeschreibung:** Eine neue ZK wird mit einer Ressource (Auto) und einem Verein/Sharepad-Eintrag verknüpft.

- Die ZK erhält beim Zusammenbau/Flashen eine eindeutige ID, vorgesehen ist die **IMEI des Modems** (als QR-Code außen auf dem Gerät; ggf. auch als Barcode/Text aufgedruckt).
- Warum IMEI: Garantiert eindeutig, unabhängig von der SIM-Karte.
- Hinweis: Das Modem ist bei der neuen Hardware eine PCI-Steckkarte und technisch tauschbar. Risikobewertung: gering.
- Der Prozess der Verknüpfung (Verheiratung ZK ↔ Ressource ↔ Verein) ist noch vollständig zu definieren.
- Ziel: Möglichst alles über das Sharepad konfigurieren, minimale Konfiguration in der ZK-Firmware.

**ZK-Schnittstellen:** noch zu definieren (Bestandteil des nächsten Termins)

---

### 7. Ressource konfigurieren / ändern (UC-07)

**Kurzbeschreibung:** Parameter einer Ressource werden geändert (z. B. Standort, Abrechnungssätze, Verhalten der ZK).

- Grob besprochen: Anlegen, Ändern, Stilllegen/Löschen von Ressourcen.
- Stilllegen statt sofortigem Löschen (wegen Abrechnungsrelevanz).
- Konfiguration soll soweit wie möglich im Sharepad erfolgen; ZK-relevante Parameter noch zu definieren.
- Abrechnungsparameter (Stunden-/Kilometersätze) sind relevant, aber außerhalb der ZK-Schnittstelle.

**ZK-Schnittstellen:** ggf. Schnittstelle für Konfigurationsübertragung an ZK (noch zu definieren)

---

### 8. Firmware-Update der ZK (UC-08)

**Kurzbeschreibung:** Firmware-Updates werden remote an ZK-Geräte verteilt.

- Firmware-Update in Kombination mit Versionsverwaltung je Ressource (welche ZK hat welche Firmware-Version).
- Details noch zu definieren.

**ZK-Schnittstellen:** noch zu definieren

---

### 9. Karte anlegen / Karte kodieren (UC-09)

**Kurzbeschreibung:** Ein neuer NFC-Chip wird kodiert und einem Nutzer zugeordnet.

- Chip wird mit einem Schlüssel kodiert; UID wird zusätzlich in Sektor 1 des Chips geschrieben.
- Prozess im Zusammenhang mit der Führerscheinkontrolle (physische Prüfung durch den Verein).
- Details (wer kodiert, welche Hardware, welcher Prozess) noch zu definieren.

**ZK-Schnittstellen:** keine direkte ZK-Schnittstelle; betrifft Sharepad-Stammdaten und physischen Prozess.

---

### 10. Nutzer anlegen / Nutzer einem Mitglied zuordnen (UC-10)

**Kurzbeschreibung:** Ein neuer Nutzer wird im System angelegt und einem oder mehreren Mitgliedern zugeordnet.

- Nutzer kann sich selbst vorab anlegen (Login-Daten); erhält dann einen QR-Code.
- Zuordnung zu Mitglied/Verein erfolgt beim Aufnahmeprozess (z. B. QR-Code scannen durch Administrator).
- Ein Nutzer kann mehreren Vereinen und Mitgliedern zugeordnet sein.
- Die UID des NFC-Chips wird ausschließlich durch den Administrator des jeweiligen Vereins gepflegt (Sichtbarkeit und Schreibrecht eingeschränkt).
- Berechtigungskonzept (wer darf wen hinzufügen) noch zu definieren.

**ZK-Schnittstellen:** keine direkte ZK-Schnittstelle; Auswirkung auf Whitelist-Sync.

---

### 11. Nutzer / Mitglied ändern (UC-11)

**Kurzbeschreibung:** Stammdaten eines Nutzers oder Mitglieds werden geändert.

- Unterscheidung: Welche Felder darf der Nutzer selbst ändern, welche nur der Administrator?
- UID-Änderung (z. B. neuer Chip nach Führerscheinverlust) betrifft automatisch alle Termin-Whitelisten → Abdeckung durch Sync-Mechanismus (UC-04/UC-05).
- Empfehlung: Änderungsprotokoll (Event Sourcing) für relevante Stammdaten erwägen, um Änderungen nachvollziehen zu können. Robert: wird für initial scope zurückgestellt.

**ZK-Schnittstellen:** indirekt über Whitelist-Sync.

---

### 12. Nutzer / Mitglied / Ressource stilllegen und löschen (UC-12)

**Kurzbeschreibung:** Lifecycle-Management für Nutzer, Mitglieder und Ressourcen.

- Sofortiges Löschen nicht möglich (abrechnungsrelevante Daten müssen erhalten bleiben).
- Prozess: Stilllegen → nach adäquater Zeit (Mitglieder: ca. 6–12 Monate nach Kündigung) tatsächliches Löschen.
- Details noch zu definieren.

**ZK-Schnittstellen:** keine direkte ZK-Schnittstelle; betrifft Whitelist-Bereinigung.

---

### 13. Kilometererfassung und -übermittlung (UC-13)

**Kurzbeschreibung:** Gefahrene Kilometer werden erfasst und dem Sharepad übermittelt.

- Zwei Varianten: manuelle Eingabe im Sharepad oder automatische Übermittlung via ZK.
- Konzept automatische Übermittlung: ZK liefert Blöcke „von Uhrzeit/Datum bis Uhrzeit/Datum wurden X Kilometer gefahren" → Sharepad matcht diese mit Buchungsdaten.
- Die Kilometerfassung ist bewusst vollständig von der Zugangs- und Buchungslogik entkoppelt: Basis ist GPS-Track-Auswertung, unabhängig von Auf-/Zusperrsignalen.
- Vorteil: Funktioniert auch bei Vereinen, die ZK nicht als Zugangskontrolle verwenden.
- Details (Übertragungsformat, Frequenz, Protokollformat) werden beim nächsten Termin ausgearbeitet.

**ZK-Schnittstellen:** noch zu definieren (eigene Schnittstelle für Kilometerübertragung)

---

### 14. ZK-Protokolle hochladen, sichten und überwachen (UC-14)

**Kurzbeschreibung:** ZK-Protokolle werden periodisch hochgeladen und im Sharepad einsehbar gemacht.

- Perspektivisch (Backlog): Automatische Warnungen, z. B.:
  - Eine ZK hat seit X Tagen keine Protokolle mehr hochgeladen.
  - Eine ZK meldet „NFC-Board nicht gefunden".
  - Benachrichtigung (z. B. E-Mail) an zuständigen Administrator.

**ZK-Schnittstellen:** noch zu definieren (Upload-Schnittstelle)

---

### 15. Berechtigungen vergeben (UC-15)

**Kurzbeschreibung:** Rollen und Rechte werden Nutzern im System zugewiesen.

- Vorgeschlagenes Drei-Stufen-Modell: Anwender, Super-User, Administrator.
- Noch zu definieren: Wer darf welche Rollen vergeben? Wie ist der Prozess?
- Berechtigungen entstehen aus der Kombination Nutzer ↔ Mitglied.

**ZK-Schnittstellen:** keine direkte ZK-Schnittstelle.

---

### 16. Daten exportieren / importieren (UC-16)

**Kurzbeschreibung:** Buchungsdaten werden aus Sharepad exportiert; Kilometerdaten werden ggf. aus Drittsystemen (z. B. Access) importiert.

- Aktuell läuft der Kilometer-Ist-Zustand über Access (Eingabe) und einen Sharepad-Export.
- Zukünftig soll dies über die ZK-Schnittstelle automatisiert werden (→ UC-13).
- Details noch zu definieren.

**ZK-Schnittstellen:** indirekt (UC-13).

---

### 17. Auswertung (UC-17)

**Kurzbeschreibung:** Statistische Auswertungen über Buchungen, Nutzung, Kilometer etc.

- Gemeinsame Entscheidung: Auswertungen sind **für den aktuellen Scope nicht vorgesehen** (erstmal zurückgestellt).

**ZK-Schnittstellen:** keine.

---

## Definierte / skizzierte Schnittstellen

### S-01 – Änderungsabfrage (Leichtgewichtig / „gibt es eine Änderung?")

| Attribut | Beschreibung |
|---|---|
| **Richtung** | ZK → Sharepad |
| **Protokoll** | HTTP (unverschlüsselt; da keine Nutzdaten übertragen werden) |
| **Anfrage-Inhalt** | Ressourcen-ID + Zeitstempel des letzten Syncs |
| **Antwort-Inhalt** | Ja/Nein: Gibt es eine Änderung bei Buchungen dieser Ressource seit letztem Sync? |
| **Zweck** | Minimaler Datenverbrauch / Stromverbrauch. Wenn Antwort = Nein: kein weiterer Download nötig. |
| **Trigger** | Zyklisch vor anstehenden Terminen (engmaschig); außerhalb von Buchungsfenstern in längeren Intervallen; bei Kartenzugang mit fehlendem Termin |
| **Genutzt in UC** | UC-02, UC-03, UC-04, UC-05 |

---

### S-02 – Buchungen / Termine abrufen (HTTPS, vollständige Daten)

| Attribut | Beschreibung |
|---|---|
| **Richtung** | ZK → Sharepad (Pull) |
| **Protokoll** | HTTPS (verschlüsselt) |
| **Anfrage-Inhalt** | Ressourcen-ID (eindeutige ZK-ID, vorgesehen: IMEI des Modems) |
| **Antwort-Inhalt** | Liste der Reservierungen inkl. je Reservierung: Start/Ende, reservierungsspezifische Whitelist (UIDs der berechtigten Nutzer); außerdem globale Whitelist |
| **Struktur** | Reservierungsspezifische Whitelist ist Teil jeder Reservierung; globale Whitelist wird separat übertragen |
| **Zeitraum** | Reservierungen der nächsten 14 Tage (oder konfigurierbarer Zeitraum; ggf. Split in „nächste 14 Tage" und „aktuell laufende Buchung") |
| **Trigger** | Wird nur aufgerufen, wenn S-01 „Änderung vorhanden" zurückmeldet oder bei täglichem Vollsync |
| **Genutzt in UC** | UC-02, UC-03, UC-04, UC-05 |

---

### S-03 – Absperrdaten live übermitteln

| Attribut | Beschreibung |
|---|---|
| **Richtung** | ZK → Sharepad (Push-ähnlich: ZK sendet aktiv) |
| **Protokoll** | REST/HTTPS |
| **Inhalt** | Ressourcen-ID, Karten-UID, Uhrzeit, Datum, GPS-Koordinaten des Absperrvorgangs |
| **Zeitpunkt** | Sofort beim Absperrereignis per Karte (Live-Nachricht, kein Batch) |
| **GPS-Handling** | GPS-Koordinaten werden übermittelt und im Sharepad temporär je Buchung gespeichert. Nur die Koordinaten des letzten Absperrvorgangs vor Buchungsende werden dauerhaft als „Rückgabeort" gespeichert. Koordinaten während laufender Buchung werden nicht dauerhaft gespeichert. Datenschutzfragen (Speicherung während Buchung) sind noch mit Tabea/Jörg (Datenschutzbeauftragter) zu klären. |
| **Zweck** | Sharepad behält volle Entscheidungshoheit über Folgeverarbeitung (Abrechnung, Überziehungserkennung, Warnungen). |
| **Genutzt in UC** | UC-03 |

---

### S-04 – Kilometer-Übertragung (noch nicht vollständig definiert)

| Attribut | Beschreibung |
|---|---|
| **Richtung** | ZK → Sharepad |
| **Inhalt (Entwurf)** | Blöcke: von [Datum/Uhrzeit] bis [Datum/Uhrzeit] wurden X Kilometer gefahren |
| **Besonderheit** | Vollständig entkoppelt von Buchungs-/Zugangslogik. Sharepad matcht Kilometer-Blöcke selbst mit Buchungen. |
| **Status** | Wird beim nächsten Termin ausgearbeitet. |
| **Genutzt in UC** | UC-13 |

---

## Use Cases je Schnittstelle (Übersicht)

| Schnittstelle | Use Cases |
|---|---|
| S-01 (Änderungsabfrage) | UC-02, UC-03, UC-04, UC-05 |
| S-02 (Buchungen abrufen) | UC-02, UC-03, UC-04, UC-05 |
| S-03 (Absperrdaten live) | UC-03 |
| S-04 (Kilometer-Übertragung) | UC-13 |

---

## Architekturprinzipien (im Gespräch festgelegt)

- **Pull statt Push:** Die ZK pollt aktiv das Sharepad. Ein serverseitiger Push (z. B. über MQTT) ist wegen des Hardware-Schlafmodus und der Modem-Aufwachzeit technisch ungeeignet und wurde verworfen.
- **Zweiteilung leichtgewichtig/vollständig:** S-01 ist eine minimale HTTP-Abfrage ohne Nutzdaten (kein TLS-Overhead). Nur bei positiver Antwort folgt der aufwändige HTTPS-Download über S-02.
- **Frontend bleibt „dumm":** Möglichst wenig Logik in der ZK. Alle Entscheidungen (Buchungsverwaltung, Abrechnung, Warnungen) liegen im Sharepad.
- **ZK-ID:** Die ZK identifiziert sich anhand der IMEI des verbauten Modems. Diese ist als QR-Code auf dem Gerät aufgedruckt und unabhängig von der SIM-Karte.
- **Absperren per Karte = Indiz, kein Beweis:** Das ZK-Absperrsignal ist kein zuverlässiger Buchungsendpunkt (Nutzer kann mit Schlüssel weiterfahren). Sharepad nutzt es als Hinweis, trifft aber selbst die Entscheidung.

---

## Weitere Themen, ToDos und offene Entscheidungen

### ToDos (konkret benannt)

| # | Aufgabe | Verantwortlich |
|---|---|---|
| T-01 | Finn befragen: Stromverbrauch und Zeiten für drei Modem-Modi: (a) aktiv mit Internet, (b) aktiv ohne Internet, (c) Schlafmodus. Inkl. Übergangszeiten und Turnaround-Zeit von Schlafmodus bis erster Request beantwortet ist. | Marcel (Auftrag an Finn) |
| T-02 | Robert prüft historische Buchungsdaten: Wie häufig sind kurzfristige Stornierungen + sofortige Neubuchungen? Ziel: Ermittlung des sinnvollen Polling-Intervalls. | Robert |
| T-03 | Prozessdiagramm für UC-02 (Öffnen) und UC-03 (Schließen) überarbeiten und an den im Gespräch definierten Ablauf anpassen. | Robert / Marcel |
| T-04 | GPS-Koordinaten / Datenschutz: Klärung mit Tabea und Jörg (Datenschutzbeauftragte), ob und wie GPS-Koordinaten während einer laufenden Buchung temporär gespeichert werden dürfen. | Robert |
| T-05 | Use Case Liste mit Klaus durchgehen: Vollständige Use Case Liste (dieses Dokument) Klaus zur Durchsicht geben, gemeinsamen Termin vereinbaren um fehlende Use Cases zu identifizieren. | Marcel / Robert |
| T-06 | Nächsten Termin: Kilometererfassung und -übertragung (UC-13) im Detail ausarbeiten. | Marcel / Robert |
| T-07 | Nächsten Termin: Ressource registrieren (UC-06) und konfigurieren (UC-07) im Detail ausarbeiten inkl. ZK-Verheiratungsprozess. | Marcel / Robert |
| T-08 | Nächsten Termin: Nutzer anlegen, Nutzer einem Mitglied/Verein zuordnen (UC-10), Berechtigungskonzept (UC-15). | Marcel / Robert |

---

### Offene Entscheidungen / Fragen

| # | Frage / Entscheidung | Status |
|---|---|---|
| E-01 | Inhalt der globalen Whitelist: Nur Mitglieder des eigenen Vereins, oder alle Sharepad-Nutzer, oder Nutzer mit Quernutzer-Vereinbarung? | Offen – liegt in Roberts Entscheidungshoheit |
| E-02 | Polling-Intervalle: Konkrete Zeitwerte für die verschiedenen ZK-Zustände (außerhalb Buchung, vor Buchung, während Buchung) | Offen – abhängig von T-01 und T-02 |
| E-03 | Modem-Strategie: Modem dauerhaft mit Internet verbunden lassen (schneller, evtl. mehr Strom) vs. nur bei Bedarf aufwecken? | Offen – abhängig von T-01 |
| E-04 | Split S-02: Soll es einen separaten Endpunkt für „aktuelle Buchung jetzt" vs. „Buchungen der nächsten 14 Tage" geben? Datenmengen-Abwägung. | Grundsätzlich ja (Tendenz), noch nicht final |
| E-05 | Globale Whitelist: Separater API-Endpunkt oder zusammen mit S-02? | Tendenz: getrennte Endpunkte / Prozesse |
| E-06 | GPS-Koordinaten: Werden diese beim Absperren an Sharepad übermittelt? | Beschlossen: Ja (S-03 enthält GPS). Datenschutz-Details offen (→ T-04) |
| E-07 | ZK-ID: IMEI des Modems als eindeutige Ressourcen-ID verwenden? | Weitgehend beschlossen; Prozess der Zuordnung (UC-06) noch offen |
| E-08 | Berechtigungskonzept: Wer darf Nutzer zu Mitgliedern hinzufügen? Darf ein Nutzer sich selbst anlegen? | Offen – nächster Termin |
| E-09 | Änderungsprotokoll / Event Sourcing für Stammdaten (Nutzer, Ressourcen) | Zurückgestellt; Robert: bei Bedarf nachrüstbar |
| E-10 | Datenschutzerklärung / DSE der Vereine: Muss jeder Verein eigene DSE haben oder reicht Verweis auf Sharepad? | Tendenz: Verweis auf Sharepad-Datenschutzerklärung |

---

### Terminabsprache
- Nächster Termin: **Montag, 13. April 2026** (Woche 13.–16. April)
- Themen: UC-06, UC-07, UC-10, UC-13, Berechtigungskonzept, offene Polling-Strategie

---

*Dokument erstellt aus Gesprächsprotokoll vom 02.04.2026. Alle Inhalte basieren ausschließlich auf dem aufgezeichneten Gespräch.*