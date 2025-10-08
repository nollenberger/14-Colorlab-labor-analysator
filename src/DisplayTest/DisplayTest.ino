/*
 * TM1637 DISPLAY TEST SCRIPT
 * ==========================
 * 
 * Einfaches Test-Script um das 4-Digit 7-Segment Display zu testen
 * 
 * Hardware:
 * - Arduino Mega 2560
 * - TM1637 4-Digit 7-Segment Display
 * 
 * Verkabelung:
 * - CLK → PIN 3
 * - DIO → PIN 2
 * - VCC → 5V
 * - GND → GND
 * 
 * Version: 1.0
 * Datum: 2025-10-06
 */

#include <TM1637Display.h>

// ============== PIN DEFINITIONEN ==============
#define DISPLAY_CLK     3     // Clock Pin (korrigiert)
#define DISPLAY_DIO     2     // Data Pin (korrigiert)

// ============== OBJEKT INITIALISIERUNG ==============
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

// ============== SETUP FUNKTION ==============
void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }
  
  Serial.println("=== TM1637 DISPLAY TEST ===");
  Serial.println("Verkabelung prüfen:");
  Serial.println("CLK → PIN 3");
  Serial.println("DIO → PIN 2");
  Serial.println("VCC → 5V");
  Serial.println("GND → GND");
  Serial.println("===========================");
  
  // Display initialisieren
  Serial.println("Initialisiere Display...");
  
  // Verschiedene Helligkeiten testen
  for (int brightness = 0; brightness <= 7; brightness++) {
    Serial.println("Helligkeit: " + String(brightness) + "/7");
    display.setBrightness(brightness);
    
    // Alle Segmente an (8888)
    display.showNumberDec(8888, true);
    delay(1000);
  }
  
  // Optimale Helligkeit setzen
  display.setBrightness(0x0f);  // Maximum
  Serial.println("Helligkeit auf Maximum gesetzt");
  
  Serial.println("===========================");
  Serial.println("Display Test startet...");
}

// ============== HAUPT LOOP ==============
void loop() {
  
  // Test 1: Alle Segmente an
  Serial.println("Test 1: Alle Segmente (8888)");
  display.showNumberDec(8888, true);
  delay(2000);
  
  // Test 2: Display löschen
  Serial.println("Test 2: Display löschen");
  display.clear();
  delay(1000);
  
  // Test 3: Zahlen 0-9 durchlaufen
  Serial.println("Test 3: Zahlen 0-9");
  for (int i = 0; i <= 9; i++) {
    Serial.println("Zeige: " + String(i));
    display.showNumberDec(i, false);
    delay(500);
  }
  
  // Test 4: Große Zahlen
  Serial.println("Test 4: Große Zahlen");
  int numbers[] = {1234, 5678, 9012, 3456, 7890};
  for (int i = 0; i < 5; i++) {
    Serial.println("Zeige: " + String(numbers[i]));
    display.showNumberDec(numbers[i], false);
    delay(1000);
  }
  
  // Test 5: Custom Segmente - HOLD
  Serial.println("Test 5: HOLD");
  uint8_t holdData[] = {0x76, 0x3f, 0x38, 0x5e}; // H-O-L-d
  display.setSegments(holdData);
  delay(2000);
  
  // Test 6: Custom Segmente - CODE
  Serial.println("Test 6: CODE");
  uint8_t codeData[] = {0x39, 0x3f, 0x5e, 0x79}; // C-O-d-E
  display.setSegments(codeData);
  delay(2000);
  
  // Test 7: Custom Segmente - FAIL
  Serial.println("Test 7: FAIL");
  uint8_t failData[] = {0x71, 0x77, 0x30, 0x38}; // F-A-I-L
  display.setSegments(failData);
  delay(2000);
  
  // Test 8: Custom Segmente - TEST  
  Serial.println("Test 8: TEST");
  uint8_t testData[] = {0x78, 0x79, 0x6D, 0x78}; // t-E-S-t
  display.setSegments(testData);
  delay(2000);
  
  // Test 9: Blinking Effect
  Serial.println("Test 9: Blinken");
  for (int i = 0; i < 6; i++) {
    display.showNumberDec(8888, true);
    delay(300);
    display.clear();
    delay(300);
  }
  
  // Test 10: Position Test - einzelne Digits
  Serial.println("Test 10: Einzelne Positionen");
  display.clear();
  
  // Position 0 (links)
  Serial.println("Position 0: A");
  uint8_t segA[] = {0x77};
  display.setSegments(segA, 1, 0);  // 'A' an Position 0
  delay(1000);
  
  // Position 1
  Serial.println("Position 1: B");
  uint8_t segB[] = {0x7c};
  display.setSegments(segB, 1, 1);  // 'b' an Position 1
  delay(1000);
  
  // Position 2
  Serial.println("Position 2: C");
  uint8_t segC[] = {0x39};
  display.setSegments(segC, 1, 2);  // 'C' an Position 2
  delay(1000);
  
  // Position 3 (rechts)
  Serial.println("Position 3: d");
  uint8_t segD[] = {0x5e};
  display.setSegments(segD, 1, 3);  // 'd' an Position 3
  delay(1000);
  
  // Test 11: Colon Test (falls vorhanden)
  Serial.println("Test 11: Mit Doppelpunkt");
  display.showNumberDecEx(1234, 0b11100000, false);  // Alle Colons an
  delay(2000);
  
  display.showNumberDecEx(1234, 0b01000000, false);  // Nur mittlerer Colon
  delay(2000);
  
  display.showNumberDec(1234, false);  // Ohne Colon
  delay(2000);
  
  Serial.println("===========================");
  Serial.println("Ein kompletter Test-Zyklus beendet");
  Serial.println("Test wiederholt sich...");
  Serial.println("===========================");
  delay(3000);
}

/*
 * FEHLERBEHEBUNG:
 * ===============
 * 
 * Falls das Display nichts zeigt:
 * 1. Verkabelung prüfen (CLK/DIO vertauscht?)
 * 2. Stromversorgung prüfen (5V, genug Ampere?)
 * 3. Display defekt?
 * 4. Arduino Pin defekt?
 * 
 * Falls nur einzelne Segmente funktionieren:
 * - Display wahrscheinlich defekt
 * 
 * Falls Display sehr schwach leuchtet:
 * - Stromversorgung zu schwach
 * - Helligkeit zu niedrig
 * 
 * Serial Monitor zeigt alle Tests und deren Status!
 */