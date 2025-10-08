/*
 * SOUND & LCD TEST SCRIPT
 * ======================
 * 
 * Dieses Script testet alle 9 Sounds der Reihe nach und 
 * das LCD Display basierend auf EscapeRoom_FINAL
 * 
 * Hardware:
 * - Arduino Mega 2560
 * - DFPlayer Mini + SD-Karte mit 9 MP3 Dateien
 * - 4-Digit 7-Segment Display (TM1637)
 * 
 * Version: 1.0
 * Datum: 2025-09-28
 */

// ============== BIBLIOTHEKEN ==============
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

// ============== PIN DEFINITIONEN (genau wie in EscapeRoom_FINAL) ==============
// Display
#define DISPLAY_CLK 2
#define DISPLAY_DIO 3

// MP3 Player - EXAKT wie in EscapeRoom_FINAL!
#define MP3_RX 10  // Genau wie in EscapeRoom_FINAL
#define MP3_TX 11  // Genau wie in EscapeRoom_FINAL

// Start Button für Test
#define START_BUTTON 4
#define BUZZER_PIN 5  // Feedback

// ============== DEBUG EINSTELLUNGEN ==============
bool DEBUG_ENABLED = true;  // Debug an/aus

// ============== OBJEKT INITIALISIERUNGEN ==============
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFPlayerMini_Fast mp3Player;

// ============== TEST VARIABLEN ==============
int currentSoundTest = 1;
bool testRunning = false;
unsigned long testStartTime = 0;

// ============== SOUND INFORMATIONEN ==============
String soundDescriptions[9] = {
  "001.mp3 - BLAU Inkubationsergebnis",       // Sound 1
  "002.mp3 - ROT Inkubationsergebnis",        // Sound 2
  "003.mp3 - GRUEN Inkubationsergebnis",      // Sound 3
  "004.mp3 - GELB Inkubationsergebnis",       // Sound 4
  "005.mp3 - SAMPLE Demo Probe",              // Sound 5
  "006.mp3 - Kein Gefaess erkannt",           // Sound 6
  "007.mp3 - Keypad aktiviert (reserviert)",  // Sound 7
  "008.mp3 - Rechner faehrt hoch",            // Sound 8
  "009.mp3 - Fehlerton"                       // Sound 9
};

// ============== SETUP FUNKTION ==============
void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }

  debugPrint("=== SOUND & LCD TEST GESTARTET ===");
  debugPrint("Version 1.0 - Basiert auf EscapeRoom_FINAL");
  debugPrint("=====================================");

  // ============== PIN MODI SETZEN ==============
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ============== DISPLAY TEST ==============
  debugPrint("Teste LCD Display...");
  display.setBrightness(0x0f);

  // Display Test Sequenz
  debugPrint("Display Test 1: Zeige 'TEST'");
  showTEST();
  delay(2000);

  debugPrint("Display Test 2: Zeige 'HOLD'");
  showHOLD();
  delay(2000);

  debugPrint("Display Test 3: Zeige 'CODE'");
  showCODE();
  delay(2000);

  debugPrint("Display Test 4: Zeige 'FAIL'");
  showFAIL();
  delay(2000);

  debugPrint("Display Test 5: Zeige Nummern 1-9");
  for (int i = 1; i <= 9; i++) {
    showNumber(i);
    delay(1000);
  }

  debugPrint("✅ LCD Display Test abgeschlossen");

  // ============== MP3 PLAYER INITIALISIERUNG ==============
  debugPrint("Initialisiere MP3 Player...");
  mp3Serial.begin(9600);
  delay(1000);

  if (mp3Player.begin(mp3Serial)) {
    debugPrint("✅ MP3 Player erfolgreich initialisiert");
    mp3Player.volume(25);  // Lautstärke 0-30 (wie in EscapeRoom_FINAL)
    delay(100);

    // MP3 Player Info
    debugPrint("MP3 Player Lautstärke: 25/30");
    debugPrint("RX Pin: " + String(MP3_RX) + ", TX Pin: " + String(MP3_TX));

  } else {
    debugPrint("❌ MP3 Player FEHLER!");
    debugPrint("❌ Prüfe Verkabelung (wie in EscapeRoom_FINAL):");
    debugPrint("   - RX (Player) → PIN 11 (Arduino)");
    debugPrint("   - TX (Player) → PIN 10 (Arduino)");
    debugPrint("   - VCC → 5V, GND → GND");
    debugPrint("   - SD-Karte mit 001.mp3 bis 009.mp3?");
  }

  // ============== SYSTEM BEREIT ==============
  showHOLD();
  debugPrint("=====================================");
  debugPrint("✅ SYSTEM BEREIT");
  debugPrint("✅ Drücke START Button für Sound-Test");
  debugPrint("✅ Alle 9 Sounds werden getestet:");
  for (int i = 0; i < 9; i++) {
    debugPrint("   " + String(i + 1) + ": " + soundDescriptions[i]);
  }
  debugPrint("=====================================");

  // Erfolgs-Beep (wie in EscapeRoom_FINAL)
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

// ============== HAUPT LOOP ==============
void loop() {
  // Prüfe Start Button
  if (digitalRead(START_BUTTON) == LOW && !testRunning) {
    delay(50);  // Entprellung
    if (digitalRead(START_BUTTON) == LOW) {
      startSoundTest();
    }
  }

  // Verfolge laufenden Test
  if (testRunning) {
    handleSoundTest();
  }

  delay(100);
}

// ============== SOUND TEST STARTEN ==============
void startSoundTest() {
  testRunning = true;
  currentSoundTest = 1;

  debugPrint("🚀 STARTE KOMPLETTEN SOUND TEST");
  debugPrint("📢 Teste alle 9 Sounds der Reihe nach...");

  // Start-Beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  playSoundTest(currentSoundTest);
}

// ============== EINZELNEN SOUND TESTEN ==============
void playSoundTest(int soundNumber) {
  debugPrint("=====================================");
  debugPrint("🔊 TESTE SOUND " + String(soundNumber) + "/9");
  debugPrint("📝 " + soundDescriptions[soundNumber - 1]);
  debugPrint("🎵 mp3Player.play(" + String(soundNumber) + ")");

  // Display zeigt Sound Nummer
  showNumber(soundNumber);

  // Sound abspielen
  testStartTime = millis();
  mp3Player.play(soundNumber);

  debugPrint("⏰ Sound gestartet um: " + String(testStartTime) + "ms");
  debugPrint("⏳ Warte 8 Sekunden für Wiedergabe...");
}

// ============== SOUND TEST VERWALTEN ==============
void handleSoundTest() {
  // Warte 8 Sekunden pro Sound
  if (millis() - testStartTime >= 8000) {

    debugPrint("✅ Sound " + String(currentSoundTest) + " Test beendet");

    currentSoundTest++;

    if (currentSoundTest <= 9) {
      // Nächster Sound
      debugPrint("➡️ Weiter zu Sound " + String(currentSoundTest));
      delay(1000);  // Pause zwischen Sounds
      playSoundTest(currentSoundTest);
    } else {
      // Test beendet
      finishSoundTest();
    }
  }
}

// ============== SOUND TEST BEENDEN ==============
void finishSoundTest() {
  testRunning = false;

  debugPrint("=====================================");
  debugPrint("🎉 SOUND TEST KOMPLETT ABGESCHLOSSEN!");
  debugPrint("✅ Alle 9 Sounds wurden getestet");
  debugPrint("=====================================");
  debugPrint("📊 TEST ZUSAMMENFASSUNG:");
  debugPrint("   - Sound 1-5: Chip Sounds (BLAU, ROT, GRÜN, GELB, SAMPLE)");
  debugPrint("   - Sound 6: Kein Gefäß erkannt");
  debugPrint("   - Sound 7: Keypad aktiviert (reserviert)");
  debugPrint("   - Sound 8: Erfolgs-Sound (Rechner fährt hoch)");
  debugPrint("   - Sound 9: Fehler-Sound");
  debugPrint("=====================================");
  debugPrint("❓ FALLS PROBLEME AUFGETRETEN:");
  debugPrint("   1. Prüfe SD-Karte: 001.mp3 bis 009.mp3 vorhanden?");
  debugPrint("   2. Prüfe MP3 Player Verkabelung (RX/TX vertauscht?)");
  debugPrint("   3. Prüfe Stromversorgung (5V, genug Ampere?)");
  debugPrint("   4. Teste mit anderem MP3 Player oder SD-Karte");
  debugPrint("=====================================");
  debugPrint("🔄 Drücke START Button für erneuten Test");

  // Zurück zu HOLD Display
  showHOLD();

  // Erfolgs-Sequenz (3x Beep)
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

// ============== DEBUG FUNKTION ==============
void debugPrint(String message) {
  if (DEBUG_ENABLED) {
    Serial.println("[DEBUG] " + message);
  }
}

// ============== DISPLAY FUNKTIONEN (wie in EscapeRoom_FINAL) ==============
void showHOLD() {
  uint8_t data[] = { 0x76, 0x3f, 0x38, 0x5e };  // H-O-L-d
  display.setSegments(data);
}

void showCODE() {
  uint8_t data[] = { 0x39, 0x3f, 0x5e, 0x79 };  // C-O-d-E
  display.setSegments(data);
}

void showFAIL() {
  uint8_t data[] = { 0x71, 0x77, 0x30, 0x38 };  // F-A-I-L
  display.setSegments(data);
}

void showTEST() {
  uint8_t data[] = { 0x78, 0x79, 0x6D, 0x78 };  // t-E-S-t
  display.setSegments(data);
}

void showNumber(int number) {
  if (number >= 1 && number <= 9) {
    display.showNumberDec(number, false);
  }
}

/*
 * ===================================================================
 *                        ANLEITUNG ZUM TESTEN
 * ===================================================================
 * 
 * VORBEREITUNG:
 * ------------
 * 1. SD-Karte mit 001.mp3 bis 009.mp3 vorbereiten
 * 2. MP3 Player Verkabelung prüfen (wie in EscapeRoom_FINAL):
 *    - RX (Player) → PIN 11 (Arduino)
 *    - TX (Player) → PIN 10 (Arduino)
 *    - VCC → 5V, GND → GND
 * 3. TM1637 Display anschließen:
 *    - CLK → PIN 2, DIO → PIN 3
 *    - VCC → 5V, GND → GND
 * 
 * DURCHFÜHRUNG:
 * ------------
 * 1. Script hochladen und Serial Monitor öffnen (9600 baud)
 * 2. Display Test läuft automatisch beim Start
 * 3. START Button drücken für Sound Test
 * 4. Alle 9 Sounds werden 8 Sekunden lang getestet
 * 5. Debug Ausgaben verfolgen
 * 
 * ERWARTETES VERHALTEN:
 * --------------------
 * - Display zeigt TEST, HOLD, CODE, FAIL und Nummern 1-9
 * - Alle 9 Sounds spielen der Reihe nach ab
 * - Serial Monitor zeigt detaillierte Debug Info
 * - Nach dem Test: 3x Beep + zurück zu HOLD
 * 
 * FEHLERBEHEBUNG:
 * --------------
 * - Kein Display: Verkabelung prüfen (CLK/DIO)
 * - Kein Sound: MP3 Player Verkabelung, SD-Karte prüfen  
 * - Debug ausschalten: DEBUG_ENABLED = false
 * 
 * ===================================================================
 */