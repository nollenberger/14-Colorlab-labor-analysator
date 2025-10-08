/*
 * ESCAPE ROOM INKUBATOR - FINALES SCRIPT
 * =====================================
 * 
 * VOLLSTÄNDIG FUNKTIONSFÄHIG - BEREIT ZUM TESTEN!
 * 
 * Hardware:
 * - Arduino Mega 2560
 * - RFID Scanner RC522 (3.3V!)
 * - 5 verschiedene RFID-Chips 
 * - 4-Kanal Relais Modul
 * - 4x4 Matrix Keypad
 * - 4-Digit 7-Segment Display (TM1637)
 * - Power-LED, Gelbe LED, Rote LED
 * - Summer/Buzzer
 * - Start Taster
 * - LED-Streifen (WS2812B)
 * - DFPlayer Mini + SD-Karte
 * - LED-Laufschrift Trigger
 * 
 * SPIELABLAUF:
 * 1. Power-LED leuchtet (Stromkontrolle)
 * 2. Display zeigt "HOLD" 
 * 3. RFID-Chip auflegen
 * 4. Start-Taster drücken → Beep
 * 5. System erkennt Chip und spielt Sound ab
 * 6. Display zeigt "CODE", gelbe LED an, LED-Streifen in Farbe
 * 7. 4-stelligen Code am Keypad eingeben
 * 8. Bei richtigem Code: LED-Laufschrift + Video-Relais
 * 9. Bei falschem Code: Summer + rote LED blinkt 3x
 * 
 * Version: 2.0 - FINAL
 * Datum: 2025-09-28
 * Bereit zum Testen!
 */

// ============== BIBLIOTHEKEN ==============
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <FastLED.h>  // Wieder aktiviert für LED-Strips

// ============== PIN DEFINITIONEN ==============
// RFID
#define RST_PIN         9
#define SS_PIN          53

// Relais (Video-Trigger)
#define RELAY1_PIN      22    // Video Trigger 1 - BLAU
#define RELAY2_PIN      24    // Video Trigger 2 - ROT
#define RELAY3_PIN      26    // Video Trigger 3 - GRÜN
#define RELAY4_PIN      28    // Video Trigger 4 - GELB

// Display & UI
#define DISPLAY_CLK     3     // Korrigiert: CLK auf PIN 3
#define DISPLAY_DIO     2     // Korrigiert: DIO auf PIN 2
#define START_BUTTON    4

// Audio & Feedback
#define BUZZER_PIN      5     // Summer/Buzzer
#define LED_STRIP_PIN   14     // ORANGE LED Lampe (Loading Animation)
#define RED_LED_PIN     7     // Fehler-LED
#define YELLOW_LED_PIN  8     // Erfolg-LED
#define POWER_LED_PIN   12    // Power-Kontrolle LED
#define SCROLLTEXT_PIN  13    // LED-Laufschrift Badge (direkt)

// MP3 Player
#define MP3_RX          10
#define MP3_TX          11

// ============== FASTLED KONFIGURATION ==============
#define NUM_LEDS           2     // 1 Strip mit 2 LEDs: LED 0 = Orange, LED 1 = Grün
#define LED_TYPE          WS2812B
#define COLOR_ORDER       GRB

// ============== KEYPAD KONFIGURATION ==============
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {30, 32, 34, 36};
byte colPins[COLS] = {38, 40, 42, 44};

// ============== OBJEKT INITIALISIERUNGEN ==============
MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFPlayerMini_Fast mp3Player;
// FastLED Array - 1 Strip mit 2 LEDs
CRGB leds[NUM_LEDS];  // LED 0 = Orange, LED 1 = Grün

// ============== SPIEL ZUSTÄNDE ==============
enum GameState {
  GAME_HOLD,           // Warten auf Start
  GAME_CODE_INPUT,     // Code-Eingabe aktiv
  GAME_PROCESSING      // Verarbeitung
};

GameState currentState = GAME_HOLD;
String currentCode = "";
byte currentSampleType = 0;

// ============== TIMING VARIABLEN ==============
unsigned long lastButtonPress = 0;
unsigned long lastBlinkTime = 0;
bool ledState = false;
int blinkCount = 0;
bool isBlinking = false;

// Taster-Entprellung
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long buttonPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// LED-Laufschrift Timing
bool scrollTextActive = false;
unsigned long scrollTextStartTime = 0;
const unsigned long SCROLL_TEXT_DURATION = 13000; // 13 Sekunden

// LED Animation Timing
bool ledAnimationActive = false;
unsigned long ledAnimationStartTime = 0;
unsigned long lastOrangeBlinkTime = 0;
unsigned long lastGreenBlinkTime = 0;
int greenBlinkCount = 0;
bool orangeLedState = false;
bool greenLedState = false;
const unsigned long LED_ANIMATION_TOTAL_DURATION = 20000; // 20 Sekunden

// ============== RFID CHIP KONFIGURATION ==============
// ✅ ECHTE CHIP-UIDs EINGETRAGEN - BEREIT ZUM TESTEN!
// Alle 5 Chips sind konfiguriert und einsatzbereit!

struct Sample {
  byte uid[4];
  int code;
  byte soundFile;
  byte relayPin;
  String name;
};

Sample samples[5] = {
  // VEREINFACHT - Sounds direkt im Code zuweisen!
  {{0x69, 0x8B, 0xD8, 0xA2}, 1723, 0, RELAY1_PIN, "BLAU"},     // BLAU
  
  {{0xB3, 0xCC, 0x44, 0x92}, 3413, 0, RELAY2_PIN, "ROT"},      // ROT
  
  {{0xCA, 0x0A, 0xD9, 0x80}, 8312, 0, RELAY3_PIN, "GRÜN"},     // GRÜN
  
  {{0x63, 0x45, 0x44, 0x92}, 6352, 0, RELAY4_PIN, "GELB"},     // GELB
  
  {{0xC9, 0x40, 0xCF, 0xA2}, 0000, 0, 0, "SAMPLE"}             // SAMPLE
};

/*
 * ✅ CHIP KONFIGURATION ABGESCHLOSSEN:
 * 
 * CHIP 1 (BLAU):  0x69, 0x8B, 0xD8, 0xA2 → Code 1723 → Relais 1 (Pin 22)
 * CHIP 2 (ROT):   0xB3, 0xCC, 0x44, 0x92 → Code 3413 → Relais 2 (Pin 24) 
 * CHIP 3 (GRÜN):  0xCA, 0x0A, 0xD9, 0x80 → Code 8312 → Relais 3 (Pin 26)
 * CHIP 4 (GELB):  0x63, 0x45, 0x44, 0x92 → Code 6352 → Relais 4 (Pin 28)
 * CHIP 5 (SAMPLE): 0xC9, 0x40, 0xCF, 0xA2 → Code 0000 → Demo-Modus
 * 
 * 🚀 ALLE CHIPS BEREIT - SOFORT TESTBAR!
 */

// ============== SETUP FUNKTION ==============
void setup() {
  Serial.begin(9600);
  Serial.println("=== ESCAPE ROOM INKUBATOR GESTARTET ===");
  Serial.println("Version 2.0 - FINAL");
  
  // ============== PIN MODI SETZEN ==============
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(SCROLLTEXT_PIN, OUTPUT);
  // FastLED Strips werden im Setup initialisiert
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // ============== INITIAL ZUSTÄNDE ==============
  // Relais ausschalten (HIGH = OFF für die meisten Module)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  // LEDs und Summer
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(POWER_LED_PIN, HIGH);    // POWER-LED IMMER AN
  digitalWrite(SCROLLTEXT_PIN, LOW);         // Badge Pin auf LOW
  // FastLED Strips sind initial aus (schwarz)
  
  // ============== SPI UND RFID ==============
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID RC522 initialisiert");
  
  // ============== DISPLAY DEBUG ==============
  Serial.println("Teste LCD Display...");
  
  display.setBrightness(0x0f);
  
  // Display Test
  Serial.println("Zeige '8888' Test");
  display.showNumberDec(8888, true);
  delay(2000);
  
  Serial.println("Zeige 'TEST' Segments");
  uint8_t testData[] = {0x78, 0x79, 0x6D, 0x78}; // t-E-S-t
  display.setSegments(testData);
  delay(2000);
  
  Serial.println("Zeige 'HOLD'");
  showHOLD();
  
  Serial.println("Display Test abgeschlossen - HOLD angezeigt");
  
  // ============== MP3 PLAYER ==============
  mp3Serial.begin(9600);
  delay(1000);
  if (mp3Player.begin(mp3Serial)) {
    Serial.println("MP3 Player initialisiert");
    mp3Player.volume(25); // Lautstärke 0-30
    delay(100);
  } else {
    Serial.println("MP3 Player FEHLER - prüfe Verkabelung!");
  }
  
  // ============== FASTLED SETUP ==============
  FastLED.addLeds<LED_TYPE, 14, COLOR_ORDER>(leds, NUM_LEDS);  // PIN 14 direkt verwenden
  FastLED.setBrightness(80); // Helligkeit 0-255
  
  // LED Startup Test
  Serial.println("Teste FastLED Strip...");
  
  // Test: LED 0 Orange
  leds[0] = CRGB::Orange;
  leds[1] = CRGB::Black;
  FastLED.show();
  delay(1000);
  
  // Test: LED 1 Grün
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Green;
  FastLED.show();
  delay(1000);
  
  // Test: Beide LEDs
  leds[0] = CRGB::Orange;
  leds[1] = CRGB::Green;
  FastLED.show();
  delay(1000);
  
  // Alle LEDs ausschalten
  leds[0] = CRGB::Black;  // Orange LED aus
  leds[1] = CRGB::Black;  // Grüne LED aus
  FastLED.show();
  
  Serial.println("✅ FastLED Strip getestet (PIN 14, 2 LEDs: Orange + Grün)");
  
  // ============== SYSTEM BEREIT ==============
  Serial.println("===========================================");
  Serial.println("✅ SYSTEM VOLLSTÄNDIG INITIALISIERT");
  Serial.println("✅ Power-LED: AN");
  Serial.println("✅ Display: HOLD");
  Serial.println("✅ Bereit für RFID-Chips!");
  Serial.println("===========================================");
  
  // Kurzer Erfolgs-Beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

// ============== HAUPT LOOP ==============
void loop() {
  handleStateMachine();
  handleBlinking();
  handleScrollText(); // LED-Laufschrift Timing
  handleLoadingAnimation(); // LED Loading Animation
  delay(50); // Stabilität
}

// ============== STATE MACHINE ==============
void handleStateMachine() {
  switch (currentState) {
    case GAME_HOLD:
      handleHoldState();
      break;
      
    case GAME_CODE_INPUT:
      handleCodeInputState();
      break;
      
    case GAME_PROCESSING:
      handleProcessingState();
      break;
  }
}

// ============== HOLD ZUSTAND (WARTEN) ==============
void handleHoldState() {
  // Prüfe auf Start-Taster
  if (checkButtonPressed()) {
    playStartBeep();
    
    // Prüfe RFID-Chip
    byte detectedSample = checkForSample();
    if (detectedSample == 0) {
      // Kein Chip oder unbekannter Chip
      playFailSequence();
    } else {
      // Chip erkannt!
      currentSampleType = detectedSample;
      startSampleSequence(detectedSample);
    }
  }
}

// ============== CODE EINGABE ZUSTAND ==============
void handleCodeInputState() {
  char key = keypad.getKey();
  
  if (key) {
    if (key >= '0' && key <= '9') {
      currentCode += key;
      Serial.println("Code-Eingabe: " + currentCode);
      
      // ANZEIGE AUF DISPLAY: Zeige eingegebenen Code
      if (currentCode.length() == 1) {
        display.showNumberDec(currentCode.toInt(), false);  // "1"
      } else if (currentCode.length() == 2) {
        display.showNumberDec(currentCode.toInt(), false);  // "12"
      } else if (currentCode.length() == 3) {
        display.showNumberDec(currentCode.toInt(), false);  // "123"
      } else if (currentCode.length() == 4) {
        display.showNumberDec(currentCode.toInt(), false);  // "1234"
      }
      
      // Kurzer Beep für Tastendruck
      digitalWrite(BUZZER_PIN, HIGH);
      delay(50);
      digitalWrite(BUZZER_PIN, LOW);
      
      if (currentCode.length() == 4) {
        // 4-stelliger Code komplett
        delay(500);  // Kurz anzeigen lassen
        checkCode();
      }
    } else if (key == '*') {
      // Code zurücksetzen
      currentCode = "";
      Serial.println("Code zurückgesetzt");
      
      // Zurück zu "CODE" anzeigen
      showCODE();
      
      // Reset-Beep
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

// ============== VERARBEITUNG ZUSTAND ==============
void handleProcessingState() {
  // Wird von anderen Funktionen gesteuert
}

// ============== RFID CHIP ERKENNUNG ==============
byte checkForSample() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return 0; // Kein Chip
  }
  
  // Debug: UID ausgeben
  Serial.print("RFID erkannt - UID: ");
  for (int i = 0; i < 4; i++) {
    Serial.print("0x");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < 3) Serial.print(", ");
  }
  Serial.println();
  
  // Vergleiche mit bekannten Chips
  for (int i = 0; i < 5; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (mfrc522.uid.uidByte[j] != samples[i].uid[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      mfrc522.PICC_HaltA();
      Serial.println("🎯 CHIP " + String(i+1) + " ERKANNT: " + samples[i].name);
      return i + 1; // Rückgabe 1-5
    }
  }
  
  mfrc522.PICC_HaltA();
  Serial.println("❌ UNBEKANNTER CHIP - nicht konfiguriert!");
  return 0; // Unbekannter Chip
}

// ============== CHIP SEQUENZ STARTEN ==============
void startSampleSequence(byte sampleIndex) {
  currentState = GAME_CODE_INPUT;
  sampleIndex--; // Array Index (0-4)
  
  Serial.println("🚀 STARTE SEQUENZ FÜR: " + samples[sampleIndex].name);
  
  // MP3 Sound abspielen - DIREKTE ZUWEISUNGEN
  if (sampleIndex == 0) {
    // BLAU - ÄNDERN SIE HIER DIE SOUND-NUMMER!
    Serial.println("🔊 BLAU - Spiele Sound 3");
    mp3Player.play(3);  // <-- ÄNDERN SIE HIER FÜR BLAU
  }
  else if (sampleIndex == 1) {
    // ROT - ÄNDERN SIE HIER DIE SOUND-NUMMER!
    Serial.println("🔊 ROT - Spiele Sound 4");
    mp3Player.play(4);  // <-- ÄNDERN SIE HIER FÜR ROT
  }
  else if (sampleIndex == 2) {
    // GRÜN - ÄNDERN SIE HIER DIE SOUND-NUMMER!
    Serial.println("🔊 GRÜN - Spiele Sound 5");
    mp3Player.play(5);  // <-- ÄNDERN SIE HIER FÜR GRÜN
  }
  else if (sampleIndex == 3) {
    // GELB - ÄNDERN SIE HIER DIE SOUND-NUMMER!
    Serial.println("🔊 GELB - Spiele Sound 8");
    mp3Player.play(8);  // <-- ÄNDERN SIE HIER FÜR GELB
  }
  else if (sampleIndex == 4) {
    // SAMPLE - ÄNDERN SIE HIER DIE SOUND-NUMMER!
    Serial.println("🔊 SAMPLE - Spiele Sound 7");
    mp3Player.play(7);  // <-- ÄNDERN SIE HIER FÜR SAMPLE
  }
  
  delay(500); // MP3 Zeit geben
  
  // Gelbe LED einschalten
  digitalWrite(YELLOW_LED_PIN, HIGH);
  
  // Display auf CODE
  showCODE();
  
  // LED-Streifen wurde durch Loading LEDs ersetzt
  // Chip-spezifische LED-Anzeige nicht mehr verwendet
  
  currentCode = "";
  Serial.println("⌨️ Bereit für Code-Eingabe: " + String(samples[sampleIndex].code));
}

// ============== CODE PRÜFUNG ==============
void checkCode() {
  int enteredCode = currentCode.toInt();
  int correctCode = samples[currentSampleType - 1].code;
  
  Serial.println("🔍 Code-Prüfung: " + String(enteredCode) + " vs " + String(correctCode));
  
  if (enteredCode == correctCode) {
    // RICHTIGER CODE!
    playSuccessSequence();
  } else {
    // FALSCHER CODE!
    playErrorSequence();
  }
}

// ============== ERFOLGS SEQUENZ ==============
void playSuccessSequence() {
  currentState = GAME_PROCESSING;
  
  Serial.println("✅ ERFOLG! Richtiger Code für: " + samples[currentSampleType - 1].name);
  
  // ERFOLGS-SOUND + LED ANIMATION + DISPLAY ALLE GLEICHZEITIG!
  Serial.println("🔊 ERFOLG - Spiele Sound 8 (rechner hochfahren)");
  mp3Player.play(1);  // <-- ÄNDERN SIE HIER FÜR ERFOLG
  
  // LED ANIMATION SOFORT STARTEN (parallel zu Sound!) - KORREKTE INITIALISIERUNG
  ledAnimationActive = true;
  ledAnimationStartTime = millis();  // JETZT setzen!
  greenBlinkCount = 0;  // Reset für grüne LED
  orangeLedState = false;  // Reset für orange LED
  greenLedState = false;   // Reset für grüne LED
  lastOrangeBlinkTime = millis();  // Reset Timer
  lastGreenBlinkTime = millis();   // Reset Timer
  Serial.println("🟠 LED Animation SOFORT gestartet - parallel zu Sound! Timer gesetzt auf: " + String(ledAnimationStartTime));
  
  // ERFOLGS-ANZEIGE AUF DISPLAY (gleichzeitig mit Sound)
  showSUCCESS();  // Zeige "PASS" 
  delay(2000);  // 2 Sekunden anzeigen
  
  
  // LED-Laufschrift triggern
  triggerScrollText();
  
  delay(9000);
  // Entsprechendes Relais aktivieren
  byte relayPin = samples[currentSampleType - 1].relayPin;
  if (relayPin != 0) {
    Serial.println("🔌 Aktiviere Relais " + String(currentSampleType) + " für " + samples[currentSampleType - 1].name);
    digitalWrite(relayPin, LOW);  // Relais AN
    delay(500);
    digitalWrite(relayPin, HIGH); // Relais AUS
  }
  
  delay(2000);
  
  // Zurück zu HOLD
  resetToHold();
  
  Serial.println("🎉 SEQUENZ ERFOLGREICH ABGESCHLOSSEN!");
}

// ============== FEHLER SEQUENZ ==============
void playErrorSequence() {
  currentState = GAME_PROCESSING;
  
  Serial.println("❌ FEHLER! Falscher Code: " + currentCode);
  
  // SOFORT GLEICHZEITIG: MP3 + Rote LED + Buzzer (KEIN DELAY!)
  mp3Player.play(2);  // <-- ÄNDERN SIE HIER FÜR FEHLER  
  startBlinking(3);   // Rote LED blinken SOFORT
  digitalWrite(BUZZER_PIN, HIGH); // Buzzer SOFORT
  Serial.println("🔊 + 🔴 FEHLER: MP3 + Rote LED + Buzzer GLEICHZEITIG gestartet!");
  
  delay(100);  // Kurzer Buzzer
  digitalWrite(BUZZER_PIN, LOW);
  
  // DISPLAY BLINKEN: Code 3x blinken lassen (parallel zu allem anderen)
  Serial.println("📺 Display blinkt 3x (parallel)");
  for (int i = 0; i < 3; i++) {
    display.showNumberDec(currentCode.toInt(), false);  // Code anzeigen
    delay(400);
    display.clear();  // Ausblenden
    delay(400);
  }
  
  // WARTEN: Lass alles parallel weiterlaufen
  Serial.println("⏳ Warte 1500ms (alles läuft parallel)...");
  delay(1500);
  
  delay(1500);
  
  // Zurück zur Code-Eingabe
  showCODE();
  currentCode = "";
  currentState = GAME_CODE_INPUT;
  
  Serial.println("🔄 Bereit für neuen Code-Versuch");
}

// ============== FEHLER BEI KEINEM CHIP ==============
void playFailSequence() {
  Serial.println("❌ FEHLER! Kein gültiger Chip erkannt");
  
  // SCHRITT 1: Rote LED 3x blinken + Summer GLEICHZEITIG
  Serial.println("🔴 Starte Blinken + Beep");
  startBlinkingWithBuzzer(3);
  
  // SCHRITT 2: 1500ms warten (Blinken läuft parallel weiter)
  Serial.println("⏳ Warte 1500ms...");
  delay(1500);
  
  // KEIN-CHIP-SOUND - ÄNDERN SIE HIER DIE SOUND-NUMMER!
  Serial.println("🔊 KEIN CHIP - Spiele Sound 7 (kein gefäß erkannt)");
  mp3Player.play(7);  // <-- ÄNDERN SIE HIER FÜR KEIN CHIP
  
  // Display FAIL
  showFAIL();
  delay(2000);
  
  // Zurück zu HOLD
  resetToHold();
  
  Serial.println("🔄 Zurück zu HOLD - lege gültigen Chip auf");
}

// ============== ZURÜCK ZU HOLD ==============
void resetToHold() {
  currentState = GAME_HOLD;
  currentSampleType = 0;
  currentCode = "";
  
  // LEDs ausschalten
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Loading LEDs ausschalten
  leds[0] = CRGB::Black;  // Orange LED aus
  leds[1] = CRGB::Black;  // Grüne LED aus
  FastLED.show();
  
  // LED Animation komplett zurücksetzen
  ledAnimationActive = false;        // Animation stoppen
  ledAnimationStartTime = 0;         // Timer zurücksetzen
  greenBlinkCount = 0;               // Grün-Blink-Zähler zurücksetzen
  orangeLedState = false;            // Orange LED Status zurücksetzen
  greenLedState = false;             // Grün LED Status zurücksetzen
  lastOrangeBlinkTime = 0;           // Orange Blink Timer zurücksetzen
  lastGreenBlinkTime = 0;            // Grün Blink Timer zurücksetzen
  
  // Display zurück zu HOLD
  showHOLD();
  
  Serial.println("🔄 Zurück zu HOLD-Modus - alle LED Animation Variablen zurückgesetzt");
}

// ============== LED BLINKEN ==============
void startBlinking(int times) {
  isBlinking = true;
  blinkCount = times * 2; // *2 für an/aus Zyklen
  lastBlinkTime = millis();
  ledState = false;
}

void startBlinkingWithBuzzer(int times) {
  startBlinking(times);
  // Zusätzlicher Summer-Ton
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

void handleBlinking() {
  if (isBlinking && (millis() - lastBlinkTime > 250)) {
    ledState = !ledState;
    digitalWrite(RED_LED_PIN, ledState ? HIGH : LOW);
    
    blinkCount--;
    lastBlinkTime = millis();
    
    if (blinkCount <= 0) {
      isBlinking = false;
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

// ============== TASTER FUNKTIONEN ==============
bool checkButtonPressed() {
  bool currentButtonState = digitalRead(START_BUTTON);
  
  // Flanken-Erkennung (HIGH zu LOW = Tastendruck)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    buttonPressTime = millis();
    buttonPressed = true;
  }
  
  // Entprellung
  if (buttonPressed && (millis() - buttonPressTime > DEBOUNCE_DELAY)) {
    if (digitalRead(START_BUTTON) == LOW) {
      buttonPressed = false;
      lastButtonState = currentButtonState;
      return true; // Gültiger Tastendruck
    }
    buttonPressed = false;
  }
  
  lastButtonState = currentButtonState;
  return false;
}

void playStartBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("🔊 Start-Taster gedrückt - Beep!");
}

// ============== LED-LAUFSCHRIFT SYSTEM ==============
void triggerScrollText() {
  Serial.println("📱 Starte LED-Laufschrift Namensbadge");
  
  // BADGE START: Kurzer Impuls zum Starten
  digitalWrite(SCROLLTEXT_PIN, HIGH);
  delay(200);
  digitalWrite(SCROLLTEXT_PIN, LOW);
  Serial.println("⚡ Badge gestartet - Impuls gesendet");
  
  // Timer für automatisches Abschalten nach 13 Sekunden
  scrollTextActive = true;
  scrollTextStartTime = millis();
  
  // LED Animation wurde bereits bei Erfolgs-Sound gestartet
  Serial.println("🟠 LED Animation läuft bereits parallel zum Sound");
  
  Serial.println("✅ LED-Laufschrift gestartet - stoppt automatisch in 13 Sekunden");
  
  Serial.println("✅ LED-Laufschrift gestartet - stoppt automatisch in 13 Sekunden");
}

void handleScrollText() {
  // Prüfe ob 13 Sekunden vergangen sind
  if (scrollTextActive && (millis() - scrollTextStartTime >= SCROLL_TEXT_DURATION)) {
    // BADGE STOP: Kurzer Impuls zum Stoppen
    Serial.println("🛑 Stoppe LED-Laufschrift nach 13 Sekunden");
    
    digitalWrite(SCROLLTEXT_PIN, HIGH);
    delay(200);
    digitalWrite(SCROLLTEXT_PIN, LOW);
    Serial.println("⚡ Badge gestoppt - Stop-Impuls gesendet");
    
    digitalWrite(SCROLLTEXT_PIN, LOW);

    Serial.println("⚡ Relais AUS - LED-Laufschrift hat keinen Strom mehr");
    
    scrollTextActive = false;
    Serial.println("✅ LED-Laufschrift gestoppt");
  }
}

// ============== LED LOADING ANIMATION ==============
void handleLoadingAnimation() {
  if (!ledAnimationActive) return;
  
  unsigned long elapsed = millis() - ledAnimationStartTime;
  
  // Debug: Zeige Animation-Status (nur alle 1000ms)
  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime >= 1000) {
    Serial.println("🟠 LED Animation aktiv - Elapsed: " + String(elapsed) + "ms von " + String(LED_ANIMATION_TOTAL_DURATION) + "ms");
    lastDebugTime = millis();
  }
  
  // Phase 1: 0-4 Sekunden - Alle LEDs aus
  if (elapsed < 4000) {
    leds[0] = CRGB::Black;  // LED 0 aus
    leds[1] = CRGB::Black;  // LED 1 aus
    FastLED.show();
  }
  // Phase 2: 4-7 Sekunden - LED 0 BLAU blinkt 16x (alle ~125ms)
  else if (elapsed >= 4000 && elapsed < 7000) {
    if (millis() - lastOrangeBlinkTime >= 125) {  // 3000ms / 24 = 125ms pro Blink-Zyklus
      orangeLedState = !orangeLedState;
      if (orangeLedState) {
        leds[0] = CRGB::Blue;   // LED 0 BLAU (nicht orange!)
      } else {
        leds[0] = CRGB::Black;  // LED 0 aus
      }
      leds[1] = CRGB::Black;  // LED 1 bleibt aus
      FastLED.show();
      lastOrangeBlinkTime = millis();
    }
  }
  // Phase 3: 7-8 Sekunden - Alle LEDs aus
  else if (elapsed >= 7000 && elapsed < 8000) {
    leds[0] = CRGB::Black;  // LED 0 aus
    leds[1] = CRGB::Black;  // LED 1 aus
    FastLED.show();
  }
  // Phase 4: 8-9 Sekunden - LED 1 GRÜN blinkt 2x (alle 250ms)
  else if (elapsed >= 8000 && elapsed < 9000) {
    if (greenBlinkCount < 4) {  // 2x blinken = 4 Zustände (an-aus-an-aus)
      if (millis() - lastGreenBlinkTime >= 250) {  // Alle 250ms umschalten
        greenLedState = !greenLedState;
        if (greenLedState) {
          leds[1] = CRGB::Green;  // LED 1 GRÜN an
        } else {
          leds[1] = CRGB::Black;  // LED 1 aus
        }
        greenBlinkCount++;
        lastGreenBlinkTime = millis();
      }
    }
    leds[0] = CRGB::Black;  // LED 0 aus
    FastLED.show();
  }
  // Phase 5: 9-20 Sekunden - LED 1 dauerhaft GRÜN
  else if (elapsed >= 9000 && elapsed < LED_ANIMATION_TOTAL_DURATION) {
    leds[0] = CRGB::Black;  // LED 0 aus
    leds[1] = CRGB::Green;  // LED 1 dauerhaft GRÜN
    FastLED.show();
  }
  // Phase 6: Nach 20 Sekunden - Alle LEDs aus und Animation beenden
  else if (elapsed >= LED_ANIMATION_TOTAL_DURATION) {
    leds[0] = CRGB::Black;  // LED 0 aus
    leds[1] = CRGB::Black;  // LED 1 aus
    FastLED.show();
    ledAnimationActive = false;
    Serial.println("✅ LED Loading Animation nach 20 Sekunden beendet - alle LEDs aus");
  }
}

// ============== LED FUNCTIONS (Legacy entfernt) ==============
// setLEDStripColor Funktion wurde entfernt da FastLED nicht mehr verwendet wird

// ============== DISPLAY FUNKTIONEN ==============
void showHOLD() {
  uint8_t data[] = {0x76, 0x3f, 0x38, 0x5e}; // H-O-L-d
  display.setSegments(data);
}

void showCODE() {
  uint8_t data[] = {0x39, 0x3f, 0x5e, 0x79}; // C-O-d-E
  display.setSegments(data);
}

void showFAIL() {
  uint8_t data[] = {0x71, 0x77, 0x30, 0x38}; // F-A-I-L
  display.setSegments(data);
}

void showSUCCESS() {
  uint8_t data[] = {0x73, 0x77, 0x6D, 0x6D}; // P-A-S-S
  display.setSegments(data);
}

/*
 * ===================================================================
 *                        ANLEITUNG ZUM TESTEN
 * ===================================================================
 * 
 * SCHRITT 1: UIDs anpassen
 * -------------------------
 * 1. Verwende RFID_Programmer.ino um alle 5 Chips auszulesen
 * 2. Ersetze die UIDs in der "samples" Array (Zeile 131-141)
 * 3. Speichere und lade dieses Script hoch
 * 
 * SCHRITT 2: Hardware prüfen
 * --------------------------
 * - Power-LED sollte leuchten
 * - Display zeigt "HOLD"
 * - Serial Monitor zeigt System-Meldungen
 * 
 * SCHRITT 3: Chips testen
 * -----------------------
 * 1. Chip auf RFID-Scanner legen
 * 2. Start-Taster drücken → Beep
 * 3. System sollte Chip erkennen und Sound abspielen
 * 4. Display zeigt "CODE", gelbe LED an, LED-Streifen in Farbe
 * 5. 4-stelligen Code eingeben
 * 6. Bei richtigem Code: LED-Laufschrift + Relais
 * 
 * CHIP-CODES:
 * -----------
 * Chip 1 (BLAU): 1723
 * Chip 2 (ROT):  3413
 * Chip 3 (GRÜN): 8312
 * Chip 4 (GELB): 6352
 * Chip 5 (SAMPLE): 0 (Demo)
 * 
 * MP3 DATEIEN BENÖTIGT (FINAL KORRIGIERTES SOUND MAPPING):
 * ----------------------------------------------------------
 * 001.mp3 - "CODE aus Inkubationsergebnis BLAU eingeben" (wird über Sound 8 abgerufen - NICHT VERWENDET)
 * 002.mp3 - "CODE aus Inkubationsergebnis ROT eingeben" (wird über Sound 9 abgerufen - NICHT VERWENDET)
 * 003.mp3 - "CODE aus Inkubationsergebnis BLAU eingeben" (wird über Sound 1 abgerufen) ✅
 * 004.mp3 - "CODE aus Inkubationsergebnis ROT eingeben" (wird über Sound 2 abgerufen) ✅
 * 005.mp3 - "CODE aus Inkubationsergebnis GRÜN eingeben" (wird über Sound 3 abgerufen) ✅
 * 006.mp3 - "Kein Gefäß erkannt" (wird über Sound 7 abgerufen) ✅
 * 007.mp3 - "Keypad aktiviert" (wird über Sound 6 abgerufen - NICHT VERWENDET)
 * 008.mp3 - "CODE aus Inkubationsergebnis GELB + Rechner fährt hoch" (wird über Sound 4 abgerufen) ✅
 * 009.mp3 - "Fehlerton" (wird über Sound 5 abgerufen) ✅
 * 
 * SERIAL MONITOR COMMANDS:
 * -----------------------
 * Öffne Serial Monitor (9600 baud) für Debug-Informationen:
 * - Zeigt erkannte Chips mit UIDs
 * - Zeigt Code-Eingaben
 * - Zeigt System-Status
 * 
 * ===================================================================
 *                           VIEL ERFOLG!
 * ===================================================================
 */