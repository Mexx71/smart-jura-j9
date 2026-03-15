# SmartJuraJ9 — Jura J9.3 ESPHome Integration — Bauplan

## Ziel
Jura J9.3 Kaffeemaschine via ESPHome in Home Assistant integrieren.
Status auslesen (Kaffeezähler, Wassertank, Fehler) und Befehle senden (Kaffee brühen, Spülen, etc.).

## Hardware
- **ESP32-C6 Super Mini** (gleiche Platine wie KVM-RF-Projekt)
- Logic Level Converter 3.3V ↔ 5V (vorhanden)
- Jura 7-Pin Service Port Connector (vorhanden)
- Jura J9.3 Kaffeemaschine

## Pinout Jura 7-Pin Service Port
| Pin | Funktion | Richtung |
|-----|----------|----------|
| 1 | GND | — |
| 2 | +5V | Versorgung |
| 6 | RX | ESP → Jura |
| 7 | TX | Jura → ESP |

**WICHTIG: Pinout vor dem Anschließen mit Multimeter verifizieren!**

## ESPHome Komponente
- Repository: https://github.com/pvtex/esphome-jura-j9
- Einbindung als `external_components`
- UART-Kommunikation, Baudrate: 9600 (Jura-Standard)

## Home Assistant
- HA VM auf Unraid: 10.10.0.116
- Zieldashboard: Küche (`light.kuche_decke` Raum)

---

## Phase 0: Vorbereitung

### Werkzeug & Material checken
- [ ] Lötkolben mit feiner Spitze, Lötzinn (0.5–0.8mm)
- [ ] Flussmittel / Lötpaste
- [ ] Dritte Hand / Platinenhalter
- [ ] Multimeter (Durchgang, Spannung)
- [ ] Labornetzteil (für isoliertes Testen der Verbindung)
- [ ] Dupont-Kabel (male-male, male-female), Breadboard
- [ ] Schrumpfschlauch sortiert
- [ ] Pinheader-Leisten

### Software vorbereiten
- [ ] ESPHome Add-on in HA prüfen — Dashboard: http://10.10.0.116:6052
- [ ] USB-Treiber für ESP32 installiert?
- [ ] USB-Datenkabel bereithalten
- [ ] pvtex Repository anschauen: https://github.com/pvtex/esphome-jura-j9

---

## Phase 1: Jura Service-Port erkunden

### Sicherheit zuerst
- [ ] Kaffeemaschine AUS und **vom Strom trennen**
- [ ] Service-Port finden (7-Pin, meist unter einer Abdeckung)
- [ ] Foto vom Port machen

### Pinout verifizieren (bei eingeschalteter Maschine)
- [ ] Pin 1 (GND) → Durchgang zu Gehäuse/Masse prüfen
- [ ] Pin 2 (+5V) → Spannung messen (sollte ~5V sein)
- [ ] Pin 6 und Pin 7 identifizieren (TX/RX)
- [ ] Spannungspegel an TX messen (sollte 5V-Logik sein)
- [ ] Ergebnisse hier eintragen:

| Pin | Erwartet | Gemessen | OK? |
|-----|----------|----------|-----|
| 1 (GND) | 0V / Masse | | |
| 2 (+5V) | ~5V | | |
| 6 (RX) | — | | |
| 7 (TX) | ~5V idle | | |

---

## Phase 2: Verkabelung auf Breadboard

### Logic Level Converter verdrahten

```
ESP32 (3.3V Seite)          Level Converter          Jura (5V Seite)
─────────────────          ─────────────────          ────────────────
3.3V ──────────────────── LV                HV ────── Pin 2 (+5V)
GND  ──────────────────── GND              GND ────── Pin 1 (GND)
TX-Pin (z.B. GPIO1) ───── LV1             HV1 ────── Pin 6 (RX)
RX-Pin (z.B. GPIO0) ───── LV2             HV2 ────── Pin 7 (TX)
```

### Aufbau-Checkliste
- [ ] ESP32 auf Breadboard
- [ ] Level Converter auf Breadboard
- [ ] Low-Side (3.3V) verkabeln: LV → 3.3V, GND → GND
- [ ] High-Side (5V) verkabeln: HV → Jura 5V, GND → Jura GND
- [ ] Signalleitungen: LV1↔ESP TX, HV1↔Jura RX, LV2↔ESP RX, HV2↔Jura TX
- [ ] **Vor dem Anschließen an Jura**: Verkabelung nochmal prüfen! Falsches Anschließen kann die Jura-Platine beschädigen.

### Erster Test ohne Jura
- [ ] ESP32 per USB mit Strom versorgen
- [ ] Multimeter: 3.3V an LV-Seite?
- [ ] Labornetzteil: 5V an HV-Seite einspeisen → Level Converter funktioniert?

---

## Phase 3: ESPHome Config & erster Flash

- [ ] ESPHome-Config erstellen mit:
  - `external_components` von pvtex
  - UART auf gewählten GPIO-Pins
  - Baudrate 9600
- [ ] Erster Flash per USB
- [ ] Log beobachten (ohne Jura angeschlossen): Keine Fehler?

---

## Phase 4: Verbindung mit Jura herstellen

### Anschließen
- [ ] Kaffeemaschine ausschalten
- [ ] Connector in Service-Port stecken
- [ ] Kabel zum Breadboard führen
- [ ] Kaffeemaschine einschalten

### Kommunikation testen
- [ ] ESPHome-Log beobachten — kommen Daten von der Jura?
- [ ] Erwartete Entities prüfen:
  - [ ] Kaffeezähler (Espresso, Kaffee, etc.)
  - [ ] Wassertank-Status
  - [ ] Tresterbehälter
  - [ ] Maschinentemperatur
  - [ ] Fehlerstatus
- [ ] Falls keine Daten: TX/RX vertauscht? → Leitungen tauschen und nochmal testen

### Befehle testen
- [ ] Einfachen Read-Befehl senden (z.B. Zähler abfragen)
- [ ] **Vorsicht bei Write-Befehlen**: Tasse unterstellen vor Brüh-Befehl!
- [ ] Spül-Befehl testen (weniger kritisch)

---

## Phase 5: HA-Integration & Dashboard

- [ ] Entities in HA prüfen und benennen
- [ ] Lovelace-Karte für Küchen-Dashboard erstellen:
  - Kaffeezähler
  - Wassertank-Füllstand
  - Tresterbehälter-Status
  - Brüh-Button (mit Bestätigungsdialog!)
  - Spül-Button
- [ ] Optional: Automationen (z.B. Benachrichtigung wenn Tresterbehälter voll)

---

## Phase 6: Gehäuse (3D-Druck)

- [ ] ESP32 ausmessen (L×B×H mm, USB-Port-Position)
- [ ] Level Converter ausmessen
- [ ] Fotos machen: Bauteile + Einbauort an/neben der Kaffeemaschine
- [ ] Kabellänge bestimmen (Service-Port → Gehäuse)
- [ ] Gehäuse-Design erstellen (OpenSCAD/Fusion360)
- [ ] Drucken, einpassen, ggf. iterieren

Design-Ziele: Kompakt, runde Kanten, Kabelführung zum Service-Port, Belüftung, unauffällig neben der Maschine.

---

## Finale GPIO-Belegung
| Funktion | GPIO | Notizen |
|----------|------|---------|
| UART TX (→ Jura RX) | GPIO4 | via Level Converter LV1→HV1 |
| UART RX (← Jura TX) | GPIO5 | via Level Converter LV2←HV2 |
| Status LED (onboard) | GPIO8 | invertiert |

---

## ESPHome Config
- Pfad: `esphome/smart-jura-j9.yaml`
- Eigene externe Komponente: `esphome/components/jura_coffee/` (esp-idf kompatibel)
- Original-Referenz: pvtex/esphome-jura-j9 (ESP8266, nicht direkt kompatibel)
- Framework: esp-idf (wie KVM-RF-Projekt)
- Board: esp32-c6-devkitc-1

## Status
- 2026-03-14: Projektplan erstellt
- 2026-03-15: ESP32-C6 festgelegt, ESPHome-Config + externe Komponente erstellt (Phase 3 Software)
Plan wird laufend aktualisiert.
