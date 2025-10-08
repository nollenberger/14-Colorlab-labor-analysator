# 5 RFID-Chips Konfiguration 📟

## 🎯 Ziel
5 verschiedene RFID-Chips als Chip 1, 2, 3, 4, 5 konfigurieren mit eindeutigen Codes.

## 📋 Vorbereitung
Du brauchst:
- ✅ 5 verschiedene RFID-Chips/Karten
- ✅ RFID_Programmer Code auf Arduino
- ✅ Serial Monitor (9600 baud, "Newline")

## 🚀 Step-by-Step Vorgehen

### **Schritt 1: RFID Programmer laden**
1. Öffne `src/RFID_Programmer/RFID_Programmer.ino`
2. Lade auf Arduino Mega hoch
3. Serial Monitor öffnen (9600 baud)

### **Schritt 2: Alle 5 Chips auslesen**

#### **Chip 1 (BLAU) auslesen:**
```
Eingabe: r + ENTER
→ Chip 1 an Scanner halten
→ UID notieren (z.B. 0xAA 0xBB 0xCC 0xDD)
```

#### **Chip 2 (ROT) auslesen:**
```
Eingabe: r + ENTER  
→ Chip 2 an Scanner halten
→ UID notieren
```

#### **Chip 3 (GRÜN) auslesen:**
```
Eingabe: r + ENTER
→ Chip 3 an Scanner halten  
→ UID notieren
```

#### **Chip 4 (GELB) auslesen:**
```
Eingabe: r + ENTER
→ Chip 4 an Scanner halten
→ UID notieren
```

#### **Chip 5 (SAMPLE) auslesen:**
```
Eingabe: r + ENTER
→ Chip 5 an Scanner halten
→ UID notieren
```

### **Schritt 3: UIDs in Hauptcode eintragen**

Öffne `src/EscapeRoom_Main.ino` und ersetze die UIDs:

```cpp
Sample samples[5] = {
  {{0xAA, 0xBB, 0xCC, 0xDD}, 1723, 1, RELAY1_PIN, "BLAU"},     // DEINE CHIP 1 UID
  {{0x11, 0x22, 0x33, 0x44}, 3413, 2, RELAY2_PIN, "ROT"},      // DEINE CHIP 2 UID  
  {{0x55, 0x66, 0x77, 0x88}, 8312, 3, RELAY3_PIN, "GRÜN"},     // DEINE CHIP 3 UID
  {{0x99, 0xAA, 0xBB, 0xCC}, 6352, 4, RELAY4_PIN, "GELB"},     // DEINE CHIP 4 UID
  {{0xDD, 0xEE, 0xFF, 0x00}, 0000, 5, 0, "SAMPLE"}             // DEINE CHIP 5 UID
};
```

### **Schritt 4: Hauptcode hochladen**
1. Speichere `EscapeRoom_Main.ino`
2. Lade auf Arduino Mega hoch
3. Serial Monitor öffnen für Test

### **Schritt 5: Testen**

#### **Test mit RFID Programmer:**
1. RFID_Programmer wieder hochladen
2. `r + ENTER` eingeben
3. Chip testen → sollte jetzt anzeigen:
```
>> CHIP 1 ERKANNT = FARBE BLAU (Code: 1723)
>> CHIP 2 ERKANNT = FARBE ROT (Code: 3413)
// etc.
```

#### **Test mit Hauptprogramm:**
1. Hauptcode hochladen
2. Chip auf Scanner → Start-Taster → Code eingeben
3. Jeder Chip sollte seinen eigenen Code haben!

## 📊 Chip-Zuordnung

| Chip # | Farbe | Code | Relais | MP3 | Trigger |
|--------|-------|------|--------|-----|---------|
| **1** | BLAU | **1723** | Pin 22 | 001.mp3 | Video 1 |
| **2** | ROT | **3413** | Pin 24 | 002.mp3 | Video 2 |
| **3** | GRÜN | **8312** | Pin 26 | 003.mp3 | Video 3 |
| **4** | GELB | **6352** | Pin 28 | 004.mp3 | Video 4 |
| **5** | SAMPLE | **0** | - | 005.mp3 | Demo |

## 🔧 Beispiel-Output vom RFID Programmer

### **Bekannte Chips:**
```
=== Karte auslesen ===
Karte erkannt!
UID: 0xAA 0xBB 0xCC 0xDD
*** ERKANNTE ESCAPE ROOM KARTE ***
>> CHIP 1 ERKANNT = FARBE BLAU (Code: 1723)
```

### **Unbekannte Chips:**
```
=== Karte auslesen ===
Karte erkannt!  
UID: 0x12 0x34 0x56 0x78
*** NEUE KARTE FÜR KONFIGURATION ***
Diese Karte ist noch nicht konfiguriert.

=== FÜR HAUPTCODE VERWENDEN ===
{{0x12, 0x34, 0x56, 0x78},
Kopiere diese UID in den Hauptcode!
```

## 🎯 Vorteile dieser Methode

✅ **5 verschiedene Chips** - keine Verwirrung  
✅ **Eindeutige Codes** - jeder Chip hat seinen eigenen Code  
✅ **Einfache Erweiterung** - weitere Chips einfach hinzufügbar  
✅ **Klare Zuordnung** - Chip 1 = BLAU, Chip 2 = ROT, etc.  
✅ **Debug-freundlich** - RFID Programmer zeigt Chip-Nummer an  

## ⚡ Quick Start Checklist

- [ ] 5 verschiedene RFID-Chips bereit
- [ ] RFID_Programmer hochgeladen
- [ ] Serial Monitor getestet (9600, Newline)
- [ ] Alle 5 UIDs ausgelesen und notiert
- [ ] UIDs in Hauptcode eingetragen
- [ ] Hauptcode hochgeladen
- [ ] Test: Jeder Chip wird als Chip 1-5 erkannt

**Jetzt kannst du systematisch vorgehen!** 🚀📟