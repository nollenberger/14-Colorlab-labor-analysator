# Escape Room Arduino Projekt

## Übersicht
Dieses Projekt implementiert eine Arduino-basierte Escape Room Station mit RFID-Probenerkennung, Keypad-Eingabe und verschiedenen Audio-/Visuellen Feedbacks.

### Hauptfunktionen
- RFID-basierte Probenerkennung (5 verschiedene Gefäße)
- 4-stellige Code-Eingabe über Matrix-Keypad
- Audio-Feedback über MP3-Player
- Visuelle Anzeigen (7-Segment Display, LEDs, LED-Streifen)
- Video-Trigger über Relais-Module
- Vollautomatischer Spielablauf

## Komponenten-Liste

### Hardware
- **Arduino Mega 2560** - Hauptcontroller
- **RFID Scanner RC522** - Probenerkennung
- **4-Kanal Relais Modul** - Video-Trigger
- **4x4 Matrix Keypad** - Code-Eingabe
- **4-Digit 7-Segment Display (TM1637)** - Statusanzeige
- **Gelbe LED** - Erfolg-Anzeige
- **Rote LED** - Fehler-Anzeige
- **Power-LED** - Stromversorgung-Kontrolle (dauerhaft an)
- **Start Taster** - Spielstart (Impuls-Taster)
- **Summer/Buzzer** - Akustisches Feedback
- **LED-Laufschrift Trigger** - Externe Hardware-Trigger
- **LED-Streifen (WS2812B)** - Ambiente-Beleuchtung
- **Step-Down Converter** - Stromversorgung 12V→5V
- **DFPlayer Mini** - MP3-Audio-Ausgabe
- **microSD-Karte** - Audio-Speicher
- **Lautsprecher** - Audio-Ausgabe

### Zusätzliches Material
- Breadboards oder PCB
- Jumper-Kabel (Dupont)
- Widerstände (220Ω für LEDs, 10kΩ für Button)
- Externe 12V Stromversorgung (min. 2A)

## Installation

### 1. Arduino IDE Setup
1. Arduino IDE 1.8.0+ oder 2.0+ installieren
2. Board auswählen: `Tools > Board > Arduino Mega or Mega 2560`
3. Korrekten COM-Port auswählen

### 2. Libraries installieren
Über `Tools > Manage Libraries` folgende Libraries installieren:
- **MFRC522** (Version 1.4.10+)
- **Keypad** (Version 3.1.1+)
- **TM1637Display** (Version 1.2.0+)
- **DFPlayerMini_Fast** (Version 1.2.4+)
- **FastLED** (Version 3.6.0+)

Detaillierte Anweisungen: [docs/Libraries.md](docs/Libraries.md)

### 3. Verdrahtung
Alle Komponenten gemäß Schaltplan verdrahten:
- Detaillierte Pinbelegung: [docs/Pinbelegung.md](docs/Pinbelegung.md)
- Vollständiger Schaltplan: [schematics/Schaltplan.md](schematics/Schaltplan.md)

**WICHTIG**: RFID RC522 mit 3.3V versorgen, nicht 5V!

### 4. MP3-Dateien vorbereiten
1. microSD-Karte mit FAT32 formatieren
2. MP3-Dateien entsprechend benennen (001.mp3 bis 009.mp3)
3. Dateien direkt im Root-Verzeichnis der SD-Karte speichern

Detaillierte Sound-Liste: [sounds/MP3_Liste.md](sounds/MP3_Liste.md)

### 5. RFID-Karten programmieren
Fünf RFID-Karten mit folgenden Daten programmieren:
- **BLAU**: UID entsprechend Code (siehe Hauptcode)
- **ROT**: UID entsprechend Code
- **GRÜN**: UID entsprechend Code
- **GELB**: UID entsprechend Code
- **SAMPLE**: UID entsprechend Code

### 6. Code hochladen
1. Arduino Code öffnen: [src/EscapeRoom_Main.ino](src/EscapeRoom_Main.ino)
2. Bei Bedarf Pin-Definitionen anpassen
3. Code kompilieren und auf Arduino Mega hochladen

## Spielablauf

### Startphase
1. System startet automatisch mit Stromversorgung
2. **Power-LED leuchtet** (Stromkontrolle)
3. Display zeigt "HOLD"
4. Keypad ist deaktiviert
5. Alle anderen LEDs und Summer sind aus

### Probenerkennung
1. Spieler legt Gefäß/Probe auf RFID-Scanner
2. Spieler drückt Start-Taster (kurzer Impuls)
3. **Beep-Sound** bestätigt Tastendruck
4. System erkennt Probe und spielt entsprechenden Sound ab
4. Display wechselt zu "CODE"
5. Gelbe LED leuchtet
6. LED-Streifen zeigt entsprechende Farbe
7. Keypad wird aktiviert

### Code-Eingabe
**Codes für die Proben:**
- **BLAU**: 1723
- **ROT**: 3413
- **GRÜN**: 8312
- **GELB**: 6352
- **SAMPLE**: (Demo-Modus)

#### Bei korrektem Code:
1. "Rechner fährt hoch" Sound (3 Sekunden)
2. **LED-Laufschrift wird getriggert** (Pin 13, 200ms Impuls)
3. Entsprechendes Video-Relais wird 500ms aktiviert
4. System kehrt nach 2 Sekunden zu "HOLD" zurück

#### Bei falschem Code:
1. Fehlerton wird abgespielt
2. Rote LED blinkt 3x **+ Summer summt kurz**
3. Display bleibt auf "CODE"
4. Neuer Versuch möglich

### Fehlerfälle
#### Kein Gefäß erkannt:
1. "Kein Gefäß erkannt" Sound
2. Rote LED blinkt 3x **+ Summer summt kurz**
3. Display zeigt "FAIL" (2 Sekunden)
4. Zurück zu "HOLD"

## Hardware-Trigger Zuordnung

### Video-Trigger (Relais)
Die Relais-Ausgänge können externe Geräte steuern:
- **Relais 1** (Pin 22): Video Trigger 1 - BLAU
- **Relais 2** (Pin 24): Video Trigger 2 - ROT
- **Relais 3** (Pin 26): Video Trigger 3 - GRÜN
- **Relais 4** (Pin 28): Video Trigger 4 - GELB

### LED-Laufschrift Trigger
- **Pin 13**: LED-Laufschrift Hardware (200ms Impuls bei Erfolg)

## Konfiguration

### Arduino Code anpassen
Wichtige Einstellungen im Code `src/EscapeRoom_Main.ino`:

```cpp
// LED Strip Anzahl anpassen
#define NUM_LEDS 10

// Power-LED Pin anpassen falls nötig
#define POWER_LED_PIN 12

// LED-Laufschrift Trigger Pin anpassen falls nötig
#define SCROLLTEXT_PIN 13

// Summer Pin anpassen falls nötig
#define BUZZER_PIN 5

// MP3 Lautstärke (0-30)
mp3Player.volume(25);

// Taster-Entprellung anpassen
const unsigned long DEBOUNCE_DELAY = 50;

// RFID UIDs anpassen (falls andere Karten verwendet)
Sample samples[5] = {
  {{0x00, 0x00, 0x00, 0x01}, 1723, 1, RELAY1_PIN, "BLAU"},
  // ... weitere Samples
};
```

### Display-Helligkeit
```cpp
// Helligkeit anpassen (0x00-0x0f)
display.setBrightness(0x0f);
```

### LED-Streifen Helligkeit
```cpp
// Helligkeit anpassen (0-255)
FastLED.setBrightness(50);
```

## Troubleshooting

### Häufige Probleme

#### RFID funktioniert nicht
- Prüfen: 3.3V Versorgung (nicht 5V!)
- SPI-Verbindungen kontrollieren
- RFID-Karte in Nähe des Scanners

#### Display zeigt nichts
- CLK/DIO Pins prüfen
- 5V Versorgung kontrollieren
- Display-Library korrekt installiert?

#### Keypad reagiert nicht
- Zeilen/Spalten-Pins überprüfen
- Keypad-Library installiert?
- Kontakte des Keypads prüfen

#### Taster reagiert nicht
- Pin 4 Verbindung prüfen
- INPUT_PULLUP korrekt gesetzt?
- Taster mechanisch defekt?

#### Summer summt nicht
- Pin 5 Verbindung prüfen
- Buzzer korrekt angeschlossen (+/-)
- Aktiver oder passiver Summer?

#### Power-LED leuchtet nicht
- Pin 12 Verbindung prüfen
- LED korrekt gepolt (+/-)
- Vorwiderstand (220Ω) vorhanden?
- Arduino bekommt Strom?

#### LED-Laufschrift triggert nicht
- Pin 13 Verbindung prüfen
- Externe Hardware richtig angeschlossen?
- Trigger-Signal kommt an (Serial Monitor prüfen)?
- Externe Hardware benötigt andere Signal-Parameter?

#### Kein Sound
- SD-Karte korrekt formatiert (FAT32)?
- MP3-Dateien richtig benannt?
- DFPlayer Mini Verkabelung prüfen
- Lautsprecher angeschlossen?

#### Relais schalten nicht
- VCC/GND Verbindungen prüfen
- Richtige Relais-Logik (HIGH/LOW)?
- Externe Last am Relais angeschlossen?

### Debug-Modus
Serial Monitor (9600 baud) verwenden für:
- RFID-Erkennungsstatus
- Keypad-Eingaben
- Taster-Drück-Events
- Summer-Aktivierung
- System-Zustandsmeldungen

## Erweitérungen

### Mögliche Verbesserungen
- Zusätzliche RFID-Karten/Codes
- Timer-Funktionalität
- Highscore-System
- WiFi-Connectivity
- Smartphone-App Integration
- Zusätzliche Sensoren

### Freie Pins für Erweiterungen
Am Arduino Mega sind noch folgende Pins verfügbar:
- Digital: 14-21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45-49
- Analog: A0-A15

## Wartung

### Regelmäßige Checks
- RFID-Scanner auf Verschmutzung prüfen
- Keypad-Kontakte reinigen
- SD-Karte auf Defekte überprüfen
- Kabelverbindungen kontrollieren
- Stromversorgung überwachen

### Backup
- Arduino Code regelmäßig sichern
- SD-Karte Backup erstellen
- Dokumentation aktuell halten

## Support und Weiterentwicklung

### Projektstruktur
```
EscapeRoom_Arduino_Project/
├── src/
│   └── EscapeRoom_Main.ino     # Hauptcode
├── docs/
│   ├── Pinbelegung.md          # Pin-Zuordnung
│   └── Libraries.md            # Library-Installation
├── sounds/
│   └── MP3_Liste.md           # Sound-Dateien
├── schematics/
│   └── Schaltplan.md          # Verdrahtung
└── README.md                   # Diese Datei
```

### Versions-Info
- **Version**: 1.0
- **Datum**: 2025-09-27
- **Arduino IDE**: 1.8.0+ oder 2.0+
- **Board**: Arduino Mega 2560

### Lizenz
Dieses Projekt steht unter der MIT-Lizenz und kann frei verwendet und modifiziert werden.

### Kontakt
Bei Fragen oder Problemen können Issues im Repository erstellt werden.

---

**Viel Erfolg mit deinem Escape Room Projekt! 🎯**
