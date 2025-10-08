# Schaltplan und Verdrahtung - Escape Room Projekt

## Übersicht
Diese Dokumentation beschreibt die vollständige Verdrahtung aller Komponenten mit dem Arduino Mega 2560.

## Stromversorgung

### Hauptstromversorgung
```
Externe 12V Stromquelle
    ↓
Step-Down Converter (12V → 5V)
    ↓
5V Rail für alle Komponenten
    ↓
Arduino Mega 2560 (über VIN oder 5V Pin)
```

### Stromverteilung
- **Arduino Mega**: 5V (über VIN oder direkter 5V Pin)
- **RFID RC522**: 3.3V (vom Arduino 3.3V Pin)
- **Relais Modul**: 5V
- **LEDs**: 5V (mit Vorwiderständen)
- **Display TM1637**: 5V
- **DFPlayer Mini**: 5V
- **LED-Streifen**: 5V (separate Versorgung empfohlen bei >10 LEDs)

## Detaillierter Schaltplan

### Arduino Mega Pin-Zuordnung
```
Arduino Mega 2560
┌─────────────────────┐
│ Digital Pins        │
│ Pin 2  → TM1637 CLK │
│ Pin 3  → TM1637 DIO │
│ Pin 4  → Start Taster│
│ Pin 5  → Buzzer     │
│ Pin 6  → LED Strip  │
│ Pin 7  → Red LED    │
│ Pin 8  → Yellow LED │
│ Pin 9  → RFID RST   │
│ Pin 10 → MP3 RX     │
│ Pin 11 → MP3 TX     │
│ Pin 12 → Power LED  │
│ Pin 13 → Scroll Trg │
│ Pin 22 → Relay 1    │
│ Pin 24 → Relay 2    │
│ Pin 26 → Relay 3    │
│ Pin 28 → Relay 4    │
│ Pin 30 → Keypad R1  │
│ Pin 32 → Keypad R2  │
│ Pin 34 → Keypad R3  │
│ Pin 36 → Keypad R4  │
│ Pin 38 → Keypad C1  │
│ Pin 40 → Keypad C2  │
│ Pin 42 → Keypad C3  │
│ Pin 44 → Keypad C4  │
│                     │
│ SPI Pins            │
│ Pin 50 → RFID MISO  │
│ Pin 51 → RFID MOSI  │
│ Pin 52 → RFID SCK   │
│ Pin 53 → RFID SDA   │
│                     │
│ Power               │
│ 5V    → Components  │
│ 3.3V  → RFID VCC    │
│ GND   → All GNDs    │
└─────────────────────┘
```

## Komponenten-Verdrahtung

### 1. RFID RC522 Scanner
```
RC522        Arduino Mega
┌──────┐     ┌─────────────┐
│ VCC  │────→│ 3.3V        │
│ RST  │────→│ Pin 9       │
│ GND  │────→│ GND         │
│ IRQ  │     │ (not conn.) │
│ MISO │────→│ Pin 50      │
│ MOSI │────→│ Pin 51      │
│ SCK  │────→│ Pin 52      │
│ SDA  │────→│ Pin 53      │
└──────┘     └─────────────┘
```

**WICHTIG**: RC522 benötigt 3.3V, nicht 5V!

### 2. 4x4 Matrix Keypad
```
Keypad       Arduino Mega
┌──────┐     ┌─────────────┐
│ R1   │────→│ Pin 30      │
│ R2   │────→│ Pin 32      │
│ R3   │────→│ Pin 34      │
│ R4   │────→│ Pin 36      │
│ C1   │────→│ Pin 38      │
│ C2   │────→│ Pin 40      │
│ C3   │────→│ Pin 42      │
│ C4   │────→│ Pin 44      │
└──────┘     └─────────────┘
```

### 3. TM1637 4-Digit Display
```
TM1637       Arduino Mega
┌──────┐     ┌─────────────┐
│ VCC  │────→│ 5V          │
│ GND  │────→│ GND         │
│ CLK  │────→│ Pin 2       │
│ DIO  │────→│ Pin 3       │
└──────┘     └─────────────┘
```

### 4. 4-Kanal Relais Modul
```
Relais       Arduino Mega
┌──────┐     ┌─────────────┐
│ VCC  │────→│ 5V          │
│ GND  │────→│ GND         │
│ IN1  │────→│ Pin 22      │ (BLAU Video)
│ IN2  │────→│ Pin 24      │ (ROT Video)
│ IN3  │────→│ Pin 26      │ (GRÜN Video)
│ IN4  │────→│ Pin 28      │ (GELB Video)
└──────┘     └─────────────┘

Ausgang (NO/COM):
COM1 → Video Trigger 1
COM2 → Video Trigger 2
COM3 → Video Trigger 3
COM4 → Video Trigger 4
```

### 5. LEDs (Rot/Gelb/Power)
```
Gelbe LED (Pin 8):
Pin 8 ─────┬──── 220Ω ──── LED Anode
           │
GND ───────┴────────────── LED Kathode

Rote LED (Pin 7):
Pin 7 ─────┬──── 220Ω ──── LED Anode
           │
GND ───────┴────────────── LED Kathode

Power-LED (Pin 12):
Pin 12 ────┬──── 220Ω ──── LED Anode
           │
GND ───────┴────────────── LED Kathode

(Power-LED leuchtet dauerhaft bei Stromversorgung)
```

### 6. Summer/Buzzer
```
Summer/Buzzer (Pin 5):
Pin 5 ──────────────────── Buzzer +
GND ─────────────────────── Buzzer -

(Aktiver Summer - kein Vorwiderstand nötig)
```

### 7. LED-Laufschrift Trigger
```
LED-Laufschrift Trigger (Pin 13):
Pin 13 ──────────────────── Externe Hardware Input
GND ─────────────────────── Externe Hardware GND

(Kurzer 200ms Impuls bei erfolgreichem Code)
```

### 8. Start Taster
```
Start Taster (Momentary Switch)
┌─────────────┐
│             │
Pin 4 ─────┬─┤ │ Taster
           │ └─────────────┘
           │
       10kΩ │ (optional)
           │
GND ───────┴─── GND
```

**Pull-up Konfiguration**: INPUT_PULLUP im Code aktiviert
**Funktionsweise**: Kurzer Impuls beim Drücken, nicht dauerhaft gedrückt halten

### 9. DFPlayer Mini (MP3)
```
DFPlayer     Arduino Mega
┌──────┐     ┌─────────────┐
│ VCC  │────→│ 5V          │
│ GND  │────→│ GND         │
│ RX   │────→│ Pin 10      │
│ TX   │────→│ Pin 11      │
│ SPK+ │────→│ Speaker +   │
│ SPK- │────→│ Speaker -   │
└──────┘     └─────────────┘

microSD Karte im DFPlayer einsetzen
```

### 10. LED-Streifen (WS2812B)
```
LED Strip    Arduino Mega
┌──────┐     ┌─────────────┐
│ VCC  │────→│ 5V (extern) │
│ GND  │────→│ GND         │
│ DIN  │────→│ Pin 6       │
└──────┘     └─────────────┘
```

**Externe Stromversorgung**: Bei mehr als 10 LEDs separate 5V Versorgung verwenden

## GND-Bus Verdrahtung
Alle GND-Verbindungen müssen gemeinsam verbunden werden:

```
Arduino GND ───┬─── RFID GND
               ├─── Display GND
               ├─── Relais GND
               ├─── Taster GND
               ├─── LED GNDs (inkl. Power-LED)
               ├─── Buzzer GND
               ├─── DFPlayer GND
               ├─── LED Strip GND
               └─── Externe Stromversorgung GND
```

## Breadboard/PCB Layout Vorschlag

### Breadboard Verteilung
```
Breadboard 1 (Power Rails):
- 5V Rail (rot)
- GND Rail (schwarz)
- 3.3V Rail (orange)

Breadboard 2 (Digital Components):
- Keypad Verbindungen
- Button mit Pull-up
- LED Schaltungen

Breadboard 3 (Kommunikation):
- RFID RC522
- Display TM1637
- DFPlayer Mini
```

### Kabel-Empfehlungen
- **Rot**: 5V Verbindungen
- **Schwarz**: GND Verbindungen
- **Orange**: 3.3V (RFID)
- **Gelb**: Digitale Signale
- **Grün**: SPI Verbindungen
- **Blau**: I2C/serielle Verbindungen
- **Weiß**: Sonstige Signale

## Sicherheitshinweise

### Stromversorgung
- Externe 12V Quelle mit mindestens 2A Kapazität
- Step-Down Converter mit Überlastschutz verwenden
- Verpolung vermeiden
- Kurzschlüsse vermeiden

### Komponenten-Schutz
- Vorwiderstände für LEDs nicht vergessen (220Ω)
- RFID mit 3.3V versorgen, nicht 5V
- Relais-Modul richtig anschließen (VCC/GND nicht vertauschen)

### Verkabelung
- Kurze, saubere Verbindungen
- Dupont-Kabel oder verlötete Verbindungen
- Keine losen Verbindungen
- Kabel entsprechend der Stromstärke dimensionieren

## Troubleshooting Verkabelung

### Häufige Probleme
1. **RFID funktioniert nicht**: 5V statt 3.3V verwendet
2. **Display zeigt nichts**: CLK/DIO vertauscht
3. **Keypad reagiert nicht**: Zeilen/Spalten vertauscht
4. **Relais schalten nicht**: VCC/GND oder Logik (HIGH/LOW) falsch
5. **LEDs leuchten nicht**: Vorwiderstand fehlt oder Polung falsch

### Multimeter-Tests
- Spannungen an allen VCC-Pins messen
- Durchgang bei GND-Verbindungen prüfen
- Signale an digitalen Pins messen
- Kurzschlüsse ausschließen

## Erweiterungsmöglichkeiten
- Zusätzliche Sensoren an freie Pins
- I2C-Bus für weitere Displays
- Externe Interrupts für Buttons
- Analog-Eingänge für Potentiometer
- PWM-Ausgänge für Servo-Motoren
