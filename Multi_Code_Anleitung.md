# Multi-Code Funktionalität ✅

## 🎯 Problem gelöst!

Du hast **eine Karte** (`0xB3 0xCC 0x44 0x92`) für **mehrere Farben** verwendet. Das System wurde angepasst!

## 📋 Aktuelle Konfiguration

```cpp
Sample samples[5] = {
  {{0xB3, 0xCC, 0x44, 0x92}, 1723, 1, RELAY1_PIN, "BLAU"},  // Code 1723
  {{0xB3, 0xCC, 0x44, 0x92}, 3413, 2, RELAY2_PIN, "ROT"},   // Code 3413
  // ... weitere Karten hinzufügen
};
```

## 🚀 Wie es funktioniert

### 1. **Eine Karte - Mehrere Codes**
- **Gleiche Karte** kann für verschiedene Farben verwendet werden
- **System erkennt automatisch** welcher Code eingegeben wurde
- **Entsprechendes Relais** wird aktiviert

### 2. **Test-Ablauf**
1. **Karte auflegen** (`0xB3 0xCC 0x44 0x92`)
2. **Start-Taster drücken** → Beep
3. **Sound spielt ab** (z.B. "BLAU eingeben")
4. **Wähle deinen Code:**
   - **1723** → BLAU (Relais 1) 🔵
   - **3413** → ROT (Relais 2) 🔴
5. **System reagiert** entsprechend dem Code!

### 3. **Debug-Info**
Im Serial Monitor siehst du:
```
== MÖGLICHE CODES FÜR DIESE KARTE ==
BLAU: 1723
ROT: 3413
===========================================
```

## 🎮 Spielmechanik

### **Scenario 1: BLAU wählen**
1. Karte auflegen
2. Start-Taster
3. Code **1723** eingeben
4. ✅ "Korrekter Code für: BLAU"
5. LED-Laufschrift triggert
6. **Relais 1** (BLAU) aktiviert

### **Scenario 2: ROT wählen**  
1. **Gleiche Karte** auflegen
2. Start-Taster
3. Code **3413** eingeben
4. ✅ "Korrekter Code für: ROT"
5. LED-Laufschrift triggert
6. **Relais 2** (ROT) aktiviert

## 🔧 Weitere Karten hinzufügen

### Mit RFID Programmer
```
Befehl: r + ENTER
→ Karte auslesen
→ UID notieren
→ Im Hauptcode eintragen
```

### Beispiel für 3. Karte
Wenn du eine neue Karte ausließt (z.B. `0x12 0x34 0x56 0x78`):

```cpp
Sample samples[5] = {
  {{0xB3, 0xCC, 0x44, 0x92}, 1723, 1, RELAY1_PIN, "BLAU"},  
  {{0xB3, 0xCC, 0x44, 0x92}, 3413, 2, RELAY2_PIN, "ROT"},   
  {{0x12, 0x34, 0x56, 0x78}, 8312, 3, RELAY3_PIN, "GRÜN"},  // NEUE KARTE!
  // ...
};
```

## ⚡ Quick Test

**Sofortiger Test mit aktueller Karte:**

1. **Upload** den aktualisierten Code
2. **Serial Monitor** öffnen (9600 baud)
3. **Karte auflegen** und **Start-Taster** drücken
4. **Probiere beide Codes:**
   - `1723` → sollte BLAU erkennen
   - `3413` → sollte ROT erkennen

## 🎯 Vorteile

✅ **Flexible Codes**: Eine Karte, mehrere Möglichkeiten  
✅ **Automatische Erkennung**: System weiß welcher Code welche Farbe ist  
✅ **Debug-Info**: Siehst alle möglichen Codes im Serial Monitor  
✅ **Einfach erweiterbar**: Weitere Karten einfach hinzufügen  

Das System ist jetzt **viel flexibler**! 🚀✨