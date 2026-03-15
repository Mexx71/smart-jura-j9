# SmartJuraJ9 — Jura J9.3 ESPHome Integration — Bauplan

## Ziel
Jura J9.3 Kaffeemaschine via ESPHome in Home Assistant integrieren.
Status auslesen (Kaffeezähler, Wassertank, Fehler) und Befehle senden (Kaffee brühen, Spülen, etc.).

## Hardware

### Bauteile & Maße
| Bauteil | Maße (L×B×H) | Notizen |
|---------|-------------|---------|
| ESP32-C6 Super Mini | 27,6 × 18,1 × 5,0 mm | Ohne Pin-Header! USB-C auf 18,1mm Seite |
| Logic Level Converter 4-Ch | 14,8 × 12,3 × ~1,5 mm | Ohne Pin-Header! Direkt löten |
| Pinheader Jura 7-Pin | ~18 × 8,5mm Kunststoff | Gesamt mit Pins: 19,8mm |

### Einbau-Situation (Service-Port-Bereich)
```
Deckel (geschlossen)
│
│  ~7 mm Luft bis Deckel
│
├── Ebene 1 ──────────────── ← ESP32 + Level Converter hier (nebeneinander, flach)
│  5,3 mm
├── Oberfläche Gehäuse
│  5,5 mm
├── Ebene 2 (Oval) ───────── ← 60 × 35 mm, Pinheader + Kabel hier
│  6,6 mm
└── Boden (Service-Port)     ← Buchse, Pinheader steckt hier drin
    = 17,4 mm Gesamttiefe
```

**Einbau-Konzept:**
- ESP32 + Level Converter nebeneinander auf Ebene 1 (genug Fläche, 7mm Höhe reicht)
- Pinheader steckt im Port (Ebene 2/Boden), Kabel gehen hoch zu Ebene 1
- Ohne Pin-Header an ESP und Level Converter → direkt löten für minimale Bauhöhe
- Kein USB-C-Zugang nötig → erster Flash vor Einbau, danach OTA/WiFi
- Stromversorgung: 5V vom Jura Service-Port Pin 2

## Pinout Jura 7-Pin Service Port
Zählung von rechts nach links (Draufsicht auf Buchse):
```
(8)  7   6   5   4   3   2   1  (0)
nc  nc  +5V  nc  RxD GND TxD  nc  nc
```
Positionen 0 und 8 sind unbestückt (9er Raster, 7 Pins bestückt).

| Pin | Funktion | Richtung | Gemessen |
|-----|----------|----------|----------|
| 2 | TxD | Jura → ESP | **4,98V** ✓ (idle high) |
| 3 | GND | — | 0V (Referenz) |
| 4 | RxD | ESP → Jura | **4,98V** ✓ (Pull-up) |
| 6 | +5V | Versorgung | **4,98V** ✓ |

Quelle: pvtex/esphome-jura-j9 + HA Community Forum, bestätigt per Multimeter am 2026-03-15.

## ESPHome Komponente
- Eigene externe Komponente: `esphome/components/jura_coffee/` (esp-idf kompatibel)
- Referenz: https://github.com/pvtex/esphome-jura-j9 (ESP8266, nicht direkt kompatibel)
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

## Phase 2: Verkabelung (direkt gelötet, ohne Header)

### Verdrahtungsplan

```
ESP32-C6 (3.3V)             Level Converter            Jura Pinheader (5V)
───────────────             ───────────────            ──────────────────
3V3  ────────────────────── LV              HV ─────── Pin 6 (+5V)
GND  ────────────────────── GND            GND ─────── Pin 3 (GND)
GPIO4 (TX) ─────────────── LV1            HV1 ─────── Pin 4 (RxD)
GPIO5 (RX) ─────────────── LV2            HV2 ─────── Pin 2 (TxD)
```

**Hinweis:** LV3/LV4 und HV3/HV4 am Level Converter bleiben frei.

### Löt-Reihenfolge
1. [ ] Dünne Drähte (z.B. 26-28 AWG) zuschneiden — kurz halten!
2. [ ] **ESP32 → Level Converter** (LV-Seite): 3V3→LV, GND→GND, GPIO4→LV1, GPIO5→LV2
3. [ ] **Level Converter → Pinheader** (HV-Seite): HV→Pin6(+5V), GND→Pin3(GND), HV1→Pin4(RxD), HV2→Pin2(TxD)
4. [ ] Alle Lötstellen mit Schrumpfschlauch isolieren
5. [ ] **Vor dem Anschließen an Jura**: Verkabelung mit Multimeter durchklingeln!

### Test vor Einbau (auf dem Tisch)
- [ ] ESP32 per USB mit Strom versorgen
- [ ] Multimeter: 3.3V an LV-Seite?
- [ ] Durchgang GND prüfen
- [ ] Erster Flash per USB (Phase 3)
- [ ] Danach: Pinheader in Jura stecken, OTA-Update testen

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

## Phase 6: Einbau & Fixierung

- [ ] ESP32 + Level Converter auf Ebene 1 positionieren
- [ ] Fixierung: doppelseitiges Klebeband oder dünne Schaumstoff-Pads
- [ ] Kabelführung: Drähte vom Pinheader (Ebene 2) sauber nach oben zu Ebene 1
- [ ] Deckel muss noch schliessen (max 7mm Bauhöhe prüfen!)
- [ ] Optional: kleines 3D-gedrucktes Trägerplättchen als Montagehilfe

Design-Ziele: Unsichtbar im Maschinengehäuse, kein externes Gehäuse nötig.

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
- 2026-03-15: Hardware-Maße erfasst, Einbaukonzept festgelegt (intern, Ebene 1, ohne Pin-Header, OTA)
Plan wird laufend aktualisiert.
