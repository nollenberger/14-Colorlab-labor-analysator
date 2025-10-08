/*
 * RFID Karten Programmer
 * 
 * Dieses Hilfsskript ermöglicht es, die RFID-Karten für das Escape Room Projekt
 * zu programmieren oder ihre UIDs auszulesen.
 * 
 * WICHTIG: 
 * - Nur für programmierbare RFID-Karten (nicht alle Karten sind beschreibbar!)
 * - RC522 mit 3.3V versorgen
 * - Vorsicht: Falsche Programmierung kann Karten unbrauchbar machen
 * 
 * Autor: Escape Room Projekt
 * Version: 1.0
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          53

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Ziel-UIDs für die verschiedenen Karten
byte targetUIDs[5][4] = {
  {0x69, 0x8B, 0xD8, 0xA2r},  // BLAU
  {0xB3, 0xCC, 0x44, 0x92},  // ROT
  {0xCA, 0x0A, 0xD9, 0x80},  // GRÜN
  {0x63, 0x45, 0x44, 0x92},  // GELB
  {0xC9, 0x40, 0xCF, 0xA2}   // SAMPLE
};

String cardNames[5] = {"BLAU", "ROT", "GRÜN", "GELB", "SAMPLE"};
int cardCodes[5] = {1723, 3413, 8312, 6352, 0};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println("=== RFID Karten Programmer ===");
  Serial.println("Escape Room Projekt");
  Serial.println();
  Serial.println("Funktionen:");
  Serial.println("1. Karte auslesen (r)");
  Serial.println("2. Karte programmieren (p)");
  Serial.println("3. Alle Ziel-UIDs anzeigen (l)");
  Serial.println();
  Serial.println("Befehl eingeben und Enter drücken:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Entferne Whitespace und Newlines
    
    if (input.length() > 0) {
      char command = input.charAt(0);
      
      Serial.print("Eingabe erhalten: '");
      Serial.print(input);
      Serial.println("'");
      
      switch (command) {
        case 'r':
        case 'R':
          readCard();
          break;
          
        case 'p':
        case 'P':
          programCard();
          break;
          
        case 'l':
        case 'L':
          listTargetUIDs();
          break;
          
        default:
          Serial.println("Ungültiger Befehl. Verwende r, p oder l");
          Serial.print("Du hast eingegeben: '");
          Serial.print(command);
          Serial.println("'");
          break;
      }
      
      Serial.println();
      Serial.println("Nächsten Befehl eingeben:");
    }
  }
}

void readCard() {
  Serial.println("=== Karte auslesen ===");
  Serial.println("Karte an den Scanner halten...");
  
  // Warten auf Karte
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }
  
  Serial.println("Karte erkannt!");
  Serial.print("UID: ");
  printUID(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  
  // Prüfen ob es eine Ziel-UID ist
  int cardType = checkCardType(mfrc522.uid.uidByte);
  if (cardType >= 0) {
    Serial.println("*** ERKANNTE ESCAPE ROOM KARTE ***");
    Serial.print(">> CHIP ");
    Serial.print(cardType + 1);
    Serial.print(" ERKANNT = FARBE ");
    Serial.print(cardNames[cardType]);
    Serial.print(" (Code: ");
    Serial.print(cardCodes[cardType]);
    Serial.println(")");
  } else {
    Serial.println("*** NEUE KARTE FÜR KONFIGURATION ***");
    Serial.println("Diese Karte ist noch nicht konfiguriert.");
    Serial.println();
    Serial.println("=== FÜR HAUPTCODE VERWENDEN ===");
    Serial.print("{{");
    for (int i = 0; i < 4; i++) {
      Serial.print("0x");
      if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      if (i < 3) Serial.print(", ");
    }
    Serial.println("},");
    Serial.println("Kopiere diese UID in den Hauptcode!");
  }
  
  mfrc522.PICC_HaltA();
}

void programCard() {
  Serial.println("=== Karte programmieren ===");
  Serial.println("WARNUNG: Dies kann die Karte dauerhaft verändern!");
  Serial.println("Welche Karte soll programmiert werden?");
  
  for (int i = 0; i < 5; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(cardNames[i]);
    Serial.print(" (Code: ");
    Serial.print(cardCodes[i]);
    Serial.println(")");
  }
  
  Serial.println("Nummer eingeben (1-5) und Enter drücken:");
  Serial.flush(); // Stelle sicher dass alles gesendet wurde
  
  // Warten auf Eingabe mit verbesserter Logik
  unsigned long startTime = millis();
  String numberInput = "";
  
  while (millis() - startTime < 30000) { // 30 Sekunden Timeout
    if (Serial.available()) {
      numberInput = Serial.readStringUntil('\n');
      numberInput.trim();
      break;
    }
    delay(100);
  }
  
  if (numberInput.length() == 0) {
    Serial.println("Timeout - keine Eingabe erhalten");
    return;
  }
  
  Serial.print("Eingabe erhalten: '");
  Serial.print(numberInput);
  Serial.println("'");
  
  int choice = numberInput.toInt();
  
  if (choice < 1 || choice > 5) {
    Serial.print("Ungültige Auswahl! Du hast eingegeben: '");
    Serial.print(numberInput);
    Serial.println("'");
    Serial.println("Bitte eine Zahl von 1 bis 5 eingeben.");
    return;
  }
  
  choice--; // Array-Index (0-4)
  
  Serial.print("Programmiere Karte für: ");
  Serial.println(cardNames[choice]);
  Serial.println("Karte an den Scanner halten...");
  
  // Warten auf Karte
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }
  
  Serial.println("Karte erkannt. Programmierung wird gestartet...");
  
  // WARNUNG: Das Programmieren von UIDs funktioniert nur bei speziellen
  // programmierbaren RFID-Karten (z.B. "Magic Cards", UID changeable cards)
  
  // Für normale MIFARE Classic Karten ist das Ändern der UID nicht möglich!
  
  Serial.println("HINWEIS: UID-Programmierung nicht implementiert.");
  Serial.println("Grund: Die meisten RFID-Karten haben eine unveränderliche UID.");
  Serial.println();
  Serial.println("ALTERNATIVEN:");
  Serial.println("1. Spezielle 'Magic Cards' oder 'UID changeable' Karten kaufen");
  Serial.println("2. Die aktuellen UIDs der Karten auslesen und im Hauptcode verwenden");
  Serial.println("3. RFID-Klongerät verwenden (falls verfügbar)");
  Serial.println();
  
  Serial.print("Aktuelle UID dieser Karte: ");
  printUID(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print("Ziel-UID für ");
  Serial.print(cardNames[choice]);
  Serial.print(": ");
  printUID(targetUIDs[choice], 4);
  Serial.println();
  
  // Code zum Kopieren für den Hauptsketch ausgeben
  Serial.println("=== FÜR HAUPTCODE VERWENDEN ===");
  Serial.print("{{");
  for (int i = 0; i < 4; i++) {
    Serial.print("0x");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < 3) Serial.print(", ");
  }
  Serial.print("}, ");
  Serial.print(cardCodes[choice]);
  Serial.print(", ");
  Serial.print(choice + 1);
  Serial.print(", RELAY");
  Serial.print(choice + 1);
  Serial.print("_PIN, \"");
  Serial.print(cardNames[choice]);
  Serial.println("\"},");
  
  mfrc522.PICC_HaltA();
}

void listTargetUIDs() {
  Serial.println("=== Ziel-UIDs für Escape Room Karten ===");
  Serial.println();
  
  for (int i = 0; i < 5; i++) {
    Serial.print(cardNames[i]);
    Serial.print(" (Code ");
    Serial.print(cardCodes[i]);
    Serial.print("): ");
    printUID(targetUIDs[i], 4);
    Serial.println();
  }
  
  Serial.println();
  Serial.println("Hinweis: Diese UIDs sind im Hauptcode definiert.");
  Serial.println("Falls andere Karten verwendet werden, müssen die UIDs");
  Serial.println("im Hauptcode entsprechend angepasst werden.");
}

void printUID(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print("0x");
    if (buffer[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(buffer[i], HEX);
    if (i < bufferSize - 1) {
      Serial.print(" ");
    }
  }
}

int checkCardType(byte *uid) {
  for (int i = 0; i < 5; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (uid[j] != targetUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return i;
    }
  }
  return -1; // Keine Übereinstimmung
}

/*
 * ANLEITUNG FÜR DIE VERWENDUNG:
 * 
 * 1. Diesen Code auf den Arduino Mega hochladen
 * 2. RFID RC522 gemäß Pinbelegung anschließen (3.3V verwenden!)
 * 3. Serial Monitor öffnen (9600 baud)
 * 4. Befehle eingeben:
 *    - 'r': Karte auslesen
 *    - 'p': Karte programmieren (zeigt Anweisungen)
 *    - 'l': Ziel-UIDs anzeigen
 * 
 * WICHTIGE HINWEISE:
 * 
 * - Die meisten RFID-Karten haben unveränderliche UIDs
 * - "Magic Cards" oder spezielle programmierbare Karten sind nötig für UID-Änderungen
 * - Alternativ: Aktuelle UIDs auslesen und im Hauptcode verwenden
 * - Immer 3.3V für RC522 verwenden, nicht 5V!
 * 
 * WORKFLOW:
 * 
 * 1. Alle gewünschten Karten mit 'r' auslesen
 * 2. UIDs notieren
 * 3. Im Hauptcode (EscapeRoom_Main.ino) die UID-Arrays entsprechend anpassen
 * 4. Hauptcode hochladen und testen
 */
