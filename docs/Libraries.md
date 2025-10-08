# Benötigte Arduino Libraries

## Übersicht
Für das Escape Room Projekt werden verschiedene Libraries benötigt. Diese müssen über den Arduino Library Manager oder manuell installiert werden.

## Installation der Libraries

### Über Arduino IDE Library Manager
1. Arduino IDE öffnen
2. Tools → Manage Libraries... (oder Strg+Shift+I)
3. Nach dem Library-Namen suchen
4. Installieren klicken

## Detaillierte Library Liste

### 1. MFRC522 - RFID Library
- **Name**: MFRC522
- **Autor**: GithubCommunity
- **Version**: 1.4.10 oder höher
- **Beschreibung**: Library für RFID RC522 Module
- **Installation**: Arduino Library Manager
- **GitHub**: https://github.com/miguelbalboa/rfid

```cpp
#include <MFRC522.h>
```

### 2. Keypad Library
- **Name**: Keypad
- **Autor**: Mark Stanley, Alexander Brevig
- **Version**: 3.1.1 oder höher
- **Beschreibung**: Library für Matrix Keypads
- **Installation**: Arduino Library Manager
- **GitHub**: https://github.com/Chris--A/Keypad

```cpp
#include <Keypad.h>
```

### 3. TM1637Display Library
- **Name**: TM1637
- **Autor**: Avishay Orpaz
- **Version**: 1.2.0 oder höher
- **Beschreibung**: Library für 4-Digit 7-Segment Displays
- **Installation**: Arduino Library Manager
- **GitHub**: https://github.com/avishorp/TM1637

```cpp
#include <TM1637Display.h>
```

### 4. DFPlayerMini_Fast Library
- **Name**: DFPlayerMini_Fast
- **Autor**: PowerBroker2
- **Version**: 1.2.4 oder höher
- **Beschreibung**: Optimierte Library für DFPlayer Mini MP3 Module
- **Installation**: Arduino Library Manager
- **GitHub**: https://github.com/PowerBroker2/DFPlayerMini_Fast

```cpp
#include <DFPlayerMini_Fast.h>
```

### 5. FastLED Library
- **Name**: FastLED
- **Autor**: FastLED
- **Version**: 3.6.0 oder höher
- **Beschreibung**: Library für adressierbare LED-Streifen (WS2812B, etc.)
- **Installation**: Arduino Library Manager
- **GitHub**: https://github.com/FastLED/FastLED

```cpp
#include <FastLED.h>
```

### 6. SPI Library (Standard)
- **Name**: SPI
- **Autor**: Arduino
- **Beschreibung**: Standard SPI Communication Library (bereits in Arduino IDE enthalten)
- **Installation**: Keine Installation nötig

```cpp
#include <SPI.h>
```

### 7. SoftwareSerial Library (Standard)
- **Name**: SoftwareSerial
- **Autor**: Arduino
- **Beschreibung**: Software Serial Communication (bereits in Arduino IDE enthalten)
- **Installation**: Keine Installation nötig

```cpp
#include <SoftwareSerial.h>
```

## Alternative Libraries (Falls Probleme auftreten)

### Alternative für DFPlayer
Falls DFPlayerMini_Fast Probleme macht:
- **Name**: DFRobotDFPlayerMini
- **Autor**: DFRobot
- **Version**: 1.0.5 oder höher

### Alternative für TM1637
Falls TM1637Display nicht funktioniert:
- **Name**: TM1637TinyDisplay
- **Autor**: jasonacox
- **Version**: 1.6.0 oder höher

## Installationsreihenfolge
1. SPI (bereits vorhanden)
2. SoftwareSerial (bereits vorhanden)
3. MFRC522
4. Keypad
5. TM1637Display
6. DFPlayerMini_Fast
7. FastLED

## Kompatibilität
Alle Libraries sind kompatibel mit:
- Arduino Mega 2560
- Arduino IDE 1.8.0 oder höher
- Arduino IDE 2.0 oder höher

## Troubleshooting

### Library Konflikte
Falls es zu Konflikten kommt:
1. Alle Libraries deinstallieren
2. Arduino IDE neu starten
3. Libraries einzeln in der angegebenen Reihenfolge installieren

### Compilation Errors
Häufige Fehler und Lösungen:
- **"library not found"**: Library nicht installiert oder falscher Name
- **"multiple definitions"**: Mehrere Versionen der gleichen Library installiert
- **"avr/pgmspace.h not found"**: Veraltete Library Version

### Board-spezifische Probleme
- Stelle sicher, dass "Arduino Mega or Mega 2560" als Board ausgewählt ist
- Korrekten Port auswählen
- Bootloader eventuell neu brennen bei Problemen

## Speicherverbrauch
Das Projekt benötigt etwa:
- **Flash Memory**: ~25KB (von 256KB verfügbar)
- **SRAM**: ~3KB (von 8KB verfügbar)
- Genug Reserven für weitere Features

## Updates
Libraries sollten regelmäßig aktualisiert werden:
1. Tools → Manage Libraries
2. "Updateable" Filter verwenden
3. Updates installieren und Code testen
