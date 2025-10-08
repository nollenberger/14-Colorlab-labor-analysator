# RFID Programmer Problem - Einfache Lösung

## 🔴 Problem
Der RFID Programmer zeigt "Ungültige Auswahl!" ohne dass du etwas eingeben konntest.

## ✅ Einfachste Lösung (Empfohlen)

### Schritt 1: Deine echte RFID-Karte verwenden
Du hast bereits eine RFID-Karte mit der UID: **0xB3 0xCC 0x44 0x92**

Diese ist **bereits im Hauptcode eingetragen**! 🎉

### Schritt 2: Hauptcode hochladen
1. Öffne `src/EscapeRoom_Main.ino`
2. Lade den Code auf deinen Arduino hoch
3. **Fertig!** Deine Karte wird als **BLAU** mit Code **1723** erkannt

### Schritt 3: Testen
1. Karte auf RFID-Scanner legen
2. Start-Taster drücken
3. Code **1723** eingeben
4. Sollte funktionieren! 🚀

## 🔧 Falls du mehr Karten brauchst

### Mit RFID Programmer (repariert)
1. Verwende den **reparierten** RFID Programmer Code
2. **Serial Monitor Einstellungen**: 
   - Baud: 9600
   - Line Ending: **"Newline"** oder **"Both NL & CR"**
3. Eingabe: `r` + **ENTER** für Karte auslesen
4. UID notieren und im Hauptcode eintragen

### Ohne RFID Programmer (einfacher)
1. Lade den Hauptcode hoch
2. Öffne Serial Monitor
3. Halte verschiedene Karten an den Scanner
4. Arduino zeigt dir die UIDs im Serial Monitor
5. Kopiere die UIDs in den Code

## 📋 Aktuelle Karten-Konfiguration

```cpp
Sample samples[5] = {
  {{0xB3, 0xCC, 0x44, 0x92}, 1723, 1, RELAY1_PIN, "BLAU"},  // ✅ DEINE KARTE
  {{0x00, 0x00, 0x00, 0x02}, 3413, 2, RELAY2_PIN, "ROT"},   // ❌ ANPASSEN
  {{0x00, 0x00, 0x00, 0x03}, 8312, 3, RELAY3_PIN, "GRÜN"},  // ❌ ANPASSEN
  {{0x00, 0x00, 0x00, 0x04}, 6352, 4, RELAY4_PIN, "GELB"},  // ❌ ANPASSEN
  {{0x00, 0x00, 0x00, 0x05}, 0000, 5, 0, "SAMPLE"}          // ❌ ANPASSEN
};
```

## 🎯 Quick Start

**Für sofortigen Test:**
1. Hauptcode hochladen
2. Deine Karte (0xB3 0xCC 0x44 0x92) auf Scanner
3. Start-Taster drücken 
4. Code **1723** eingeben
5. ✅ Sollte "BLAU" erkennen und LED-Laufschrift triggern!

## 🔧 Serial Monitor Problem beheben

Falls der RFID Programmer immer noch Probleme macht:

### Arduino IDE Serial Monitor Einstellungen:
- **Baud Rate**: 9600
- **Line Ending**: "Newline" (nicht "No line ending")

### Alternative: Arduino IDE 2.0 verwenden
- Neuere Version hat bessere Serial Monitor Behandlung

### Notfall: Ohne Programmer arbeiten
Füge diese Debug-Zeile in den Hauptcode ein:

```cpp
void checkForSample() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  
  // DEBUG: UID ausgeben
  Serial.print("Erkannte UID: ");
  for (int i = 0; i < 4; i++) {
    Serial.print("0x");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < 3) Serial.print(" ");
  }
  Serial.println();
  
  // ... rest des Codes
}
```

So siehst du alle UIDs direkt im Hauptprogramm! 🔍