# MP3 Sound Dateien für DFPlayer Mini

## Übersicht
Das DFPlayer Mini Modul benötigt MP3-Dateien auf einer microSD-Karte. Die Dateien müssen in einer bestimmten Reihenfolge benannt werden.

## Dateistruktur auf SD-Karte
```
SD-Karte/
├── 001.mp3  (CODE aus dem Inkubationsergebnis der Probe BLAU eingeben)
├── 002.mp3  (CODE aus dem Inkubationsergebnis der Probe ROT eingeben)
├── 003.mp3  (CODE aus dem Inkubationsergebnis der Probe GRÜN eingeben)
├── 004.mp3  (CODE aus dem Inkubationsergebnis der Probe GELB eingeben)
├── 005.mp3  (Das ist eine Demo Probe, Prüfung nicht möglich!)
├── 006.mp3  (Kein Gefäß erkannt oder Probe falsch inkubiert)
├── 007.mp3  (Keypad aktiviert CODE eingeben)
├── 008.mp3  (Rechner fährt hoch)
└── 009.mp3  (Fehlerton Code falsch)
```

## Detaillierte Sound-Liste

### 001.mp3 - BLAU Probe
- **Inhalt**: "CODE aus dem Inkubationsergebnis der Probe BLAU eingeben"
- **Verwendung**: Wird abgespielt wenn RFID-Karte "BLAU" erkannt wird
- **Dauer**: Etwa 3-5 Sekunden
- **Trigger**: RFID Karte mit UID für BLAU erkannt

### 002.mp3 - ROT Probe
- **Inhalt**: "CODE aus dem Inkubationsergebnis der Probe ROT eingeben"
- **Verwendung**: Wird abgespielt wenn RFID-Karte "ROT" erkannt wird
- **Dauer**: Etwa 3-5 Sekunden
- **Trigger**: RFID Karte mit UID für ROT erkannt

### 003.mp3 - GRÜN Probe
- **Inhalt**: "CODE aus dem Inkubationsergebnis der Probe GRÜN eingeben"
- **Verwendung**: Wird abgespielt wenn RFID-Karte "GRÜN" erkannt wird
- **Dauer**: Etwa 3-5 Sekunden
- **Trigger**: RFID Karte mit UID für GRÜN erkannt

### 004.mp3 - GELB Probe
- **Inhalt**: "CODE aus dem Inkubationsergebnis der Probe GELB eingeben"
- **Verwendung**: Wird abgespielt wenn RFID-Karte "GELB" erkannt wird
- **Dauer**: Etwa 3-5 Sekunden
- **Trigger**: RFID Karte mit UID für GELB erkannt

### 005.mp3 - SAMPLE/Demo Probe
- **Inhalt**: "Das ist eine Demo Probe, Prüfung nicht möglich!"
- **Verwendung**: Wird abgespielt wenn RFID-Karte "SAMPLE" erkannt wird
- **Dauer**: Etwa 3-4 Sekunden
- **Trigger**: RFID Karte mit UID für SAMPLE erkannt

### 006.mp3 - Kein Gefäß erkannt
- **Inhalt**: "Kein Gefäß erkannt oder Probe falsch inkubiert"
- **Verwendung**: Wird abgespielt wenn Start-Button ohne gültige RFID-Karte gedrückt wird
- **Dauer**: Etwa 3-4 Sekunden
- **Trigger**: Start-Button ohne RFID oder unbekannte RFID

### 007.mp3 - Keypad aktiviert (RESERVIERT)
- **Inhalt**: "Keypad aktiviert CODE eingeben"
- **Verwendung**: Derzeit nicht verwendet, für zukünftige Features reserviert
- **Dauer**: Etwa 2-3 Sekunden
- **Trigger**: Manuell oder zukünftige Erweiterungen

### 008.mp3 - Rechner fährt hoch
- **Inhalt**: "Rechner fährt hoch"
- **Verwendung**: Wird abgespielt wenn korrekter Code eingegeben wurde
- **Dauer**: Etwa 2-3 Sekunden
- **Trigger**: Korrekter Code eingegeben, vor Relais-Aktivierung

### 009.mp3 - Fehlerton
- **Inhalt**: "Fehlerton Code falsch" oder anderer Fehlerton/Buzzer-Sound
- **Verwendung**: Wird abgespielt wenn falscher Code eingegeben wurde
- **Dauer**: Etwa 1-2 Sekunden
- **Trigger**: Falscher Code eingegeben

## SD-Karten Anforderungen

### Kompatibilität
- **Format**: FAT32
- **Größe**: 32GB oder kleiner (microSD/SD)
- **Klasse**: Class 10 empfohlen für bessere Performance
- **Dateisystem**: Keine Ordner - alle Dateien im Root-Verzeichnis

### MP3 Format Spezifikationen
- **Bitrate**: 8-320 kbps
- **Sample Rate**: 8 kHz bis 48 kHz
- **Kanäle**: Mono oder Stereo
- **Format**: MP3 (MPEG Audio Layer 3)

### Dateinamen-Regeln
- **Format**: 001.mp3, 002.mp3, etc.
- **Führende Nullen**: Immer 3 Stellen verwenden
- **Keine Leerzeichen**: Nur Zahlen und Punkts
- **Kleinbuchstaben**: .mp3 (nicht .MP3)

## Erstellung der Sound-Dateien

### Text-to-Speech Optionen
1. **Google Text-to-Speech** (online)
2. **Amazon Polly** (online)
3. **Balabolka** (Windows, kostenlos)
4. **eSpeak** (Linux/Windows, kostenlos)
5. **Natural Reader** (online)

### Bearbeitung
- **Audacity** (kostenlos) für Bearbeitung
- **VLC Media Player** für Format-Konvertierung
- **Online Audio Converter** für schnelle Konvertierung

### Qualitäts-Empfehlungen
- **Deutsche Stimme** wählen
- **Langsame Sprechgeschwindigkeit** für bessere Verständlichkeit
- **Lautstärke normalisieren** auf etwa -6dB
- **Stille am Anfang/Ende** minimieren (max. 0.5s)

## Installation auf SD-Karte

### Schritte
1. SD-Karte mit FAT32 formatieren
2. MP3-Dateien direkt in Root-Verzeichnis kopieren
3. SD-Karte sicher auswerfen
4. In DFPlayer Mini einsetzen
5. Arduino Code uploaden und testen

### Test der Sound-Dateien
1. Jeden Sound einzeln testen
2. Lautstärke im Code anpassen (Zeile mit mp3Player.volume())
3. Timing überprüfen (delays zwischen Sounds)

## Troubleshooting

### Häufige Probleme
- **Keine Sounds**: SD-Karte falsch formatiert oder falsche Dateinamen
- **Verzögerung**: SD-Karte zu langsam oder schlechte Qualität
- **Abbruch**: Datei zu groß oder korrupte MP3
- **Leise**: Lautstärke im Code zu niedrig eingestellt

### Debugging
```cpp
// Im Arduino Code für Debug-Ausgaben:
if (mp3Player.isPlaying()) {
  Serial.println("Sound wird abgespielt");
} else {
  Serial.println("Kein Sound aktiv");
}
```

## Backup und Versionierung
- **Backup**: Kopie aller MP3-Dateien auf Computer speichern
- **Versionierung**: Bei Änderungen neue SD-Karte vorbereiten
- **Test-Karte**: Separate SD-Karte für Tests verwenden
