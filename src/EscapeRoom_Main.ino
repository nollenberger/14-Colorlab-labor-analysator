/*
 * Escape Room Arduino Projekt
 * 
 * Komponenten:
 * - Arduino Mega 2560
 * - RFID Scanner RC522
 * - 4-Kanal Relais Modul
 * - 4x4 Matrix Keypad
 * - 4-Digit 7-Segment Display (TM1637)
 * - LEDs (gelb/rot)
 * - Start Button
 * - LED-Streifen
 * - DFPlayer Mini (MP3)
 * 
 * Autor: Escape Room Projekt
 * Version: 1.0
 * Datum: 2025-09-27
 */

// Bibliotheken einbinden
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <FastLED.h>

// Pin Definitionen
#define RST_PIN         9
#define SS_PIN          53
#define RELAY1_PIN      22    // Video Trigger 1 - BLAU
#define RELAY2_PIN      24    // Video Trigger 2 - ROT
#define RELAY3_PIN      26    // Video Trigger 3 - GRÜN
#define RELAY4_PIN      28    // Video Trigger 4 - GELB
#define DISPLAY_CLK     2
#define DISPLAY_DIO     3
#define START_BUTTON    4
#define LED_STRIP_PIN   6
#define RED_LED_PIN     7
#define YELLOW_LED_PIN  8
#define BUZZER_PIN      5     // Summer/Buzzer
#define POWER_LED_PIN   12    // Power-LED (immer an)
#define SCROLLTEXT_PIN  13    // LED-Laufschrift Trigger
#define MP3_RX          10
#define MP3_TX          11

// LED Strip Konfiguration
#define NUM_LEDS        10    // Anzahl der LEDs anpassen
#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB

// Keypad Konfiguration
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

// Objekt Initialisierungen
MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFPlayerMini_Fast mp3Player;
CRGB leds[NUM_LEDS];

// Globale Variablen
enum GameState {
  GAME_HOLD,
  GAME_WAITING_FOR_SAMPLE,
  GAME_CODE_INPUT,
  GAME_PROCESSING
};

GameState currentState = GAME_HOLD;
String currentCode = "";
byte currentSampleType = 0;
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

// RFID Karten UIDs und zugehörige Codes
struct Sample {
  byte uid[4];
  int code;
  byte soundFile;
  byte relayPin;
  String name;
};

// 5 VERSCHIEDENE RFID-CHIPS KONFIGURATION:
// Verwende den RFID_Programmer um alle 5 Chips auszulesen!
Sample samples[5] = {
  {{0x00, 0x00, 0x00, 0x01}, 1723, 1, RELAY1_PIN, "BLAU"},     // Chip 1 - BLAU (ANPASSEN!)
  {{0x00, 0x00, 0x00, 0x02}, 3413, 2, RELAY2_PIN, "ROT"},      // Chip 2 - ROT (ANPASSEN!)
  {{0x00, 0x00, 0x00, 0x03}, 8312, 3, RELAY3_PIN, "GRÜN"},     // Chip 3 - GRÜN (ANPASSEN!)
  {{0x00, 0x00, 0x00, 0x04}, 6352, 4, RELAY4_PIN, "GELB"},     // Chip 4 - GELB (ANPASSEN!)
  {{0x00, 0x00, 0x00, 0x05}, 0000, 5, 0, "SAMPLE"}             // Chip 5 - SAMPLE (ANPASSEN!)
};

// ANLEITUNG FÜR 5 VERSCHIEDENE CHIPS:
// 1. Verwende RFID_Programmer mit Befehl 'r' um alle 5 Chips einzeln auszulesen
// 2. Notiere dir die UIDs für Chip 1, 2, 3, 4, 5
// 3. Ersetze die UIDs oben mit den echten UIDs
// 4. Jeder Chip bekommt einen eigenen Code: 1723, 3413, 8312, 6352, 0

void setup() {
  Serial.begin(9600);
  
  // Pin Modi setzen
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(SCROLLTEXT_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Relais initial ausschalten (HIGH = OFF für die meisten Relais Module)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  // LEDs und Summer initial setzen
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(POWER_LED_PIN, HIGH);  // Power-LED immer an
  digitalWrite(SCROLLTEXT_PIN, LOW);  // Laufschrift-Trigger initial aus
  
  // SPI und RFID initialisieren
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Display initialisieren
  display.setBrightness(0x0f);
  display.showNumberDec(0, false); // Leeres Display
  
  // MP3 Player initialisieren
  mp3Serial.begin(9600);
  if (mp3Player.begin(mp3Serial)) {
    Serial.println("MP3 Player initialisiert");
    mp3Player.volume(25); // Lautstärke 0-30
  } else {
    Serial.println("MP3 Player Fehler");
  }
  
  // LED Strip initialisieren
  FastLED.addLeds<LED_TYPE, LED_STRIP_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  setLEDStripColor(CRGB::Black);
  
  // Initial Display auf HOLD setzen
  showHOLD();
  
  Serial.println("System initialisiert - HOLD Modus");
  Serial.println("Power-LED: AN (Pin 12)");
}

void loop() {
  handleStateMachine();
  handleBlinking();
  
  delay(50); // Kurze Pause für Stabilität
}

void handleStateMachine() {
  switch (currentState) {
    case GAME_HOLD:
      handleHoldState();
      break;
      
    case GAME_WAITING_FOR_SAMPLE:
      handleWaitingState();
      break;
      
    case GAME_CODE_INPUT:
      handleCodeInputState();
      break;
      
    case GAME_PROCESSING:
      handleProcessingState();
      break;
  }
}

void handleHoldState() {
  // Prüfe auf Start Button Druck (Taster-Entprellung)
  if (checkButtonPressed()) {
    // Start-Beep
    playStartBeep();
    
    // Prüfe ob RFID Karte vorhanden
    byte detectedSample = checkForSample();
    if (detectedSample == 0) {
      // Keine Karte oder unbekannte Karte
      playFailSequence();
    } else {
      // Gültige Karte erkannt
      currentSampleType = detectedSample;
      startSampleSequence(detectedSample);
    }
  }
}

void handleWaitingState() {
  // Warte auf neuen Start Button Druck oder andere Karte
  if (checkButtonPressed()) {
    // Start-Beep
    playStartBeep();
    
    byte detectedSample = checkForSample();
    if (detectedSample == 0) {
      playFailSequence();
    } else if (detectedSample != currentSampleType) {
      currentSampleType = detectedSample;
      startSampleSequence(detectedSample);
    }
  }
}

void handleCodeInputState() {
  char key = keypad.getKey();
  
  if (key) {
    if (key >= '0' && key <= '9') {
      currentCode += key;
      Serial.println("Eingabe: " + currentCode);
      
      if (currentCode.length() == 4) {
        // Code komplett eingegeben
        checkCode();
      }
    } else if (key == '*') {
      // Code zurücksetzen
      currentCode = "";
      Serial.println("Code zurückgesetzt");
    }
  }
}

void handleProcessingState() {
  // Wird in anderen Funktionen gesteuert
}

byte checkForSample() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return 0; // Keine Karte
  }
  
  // Vergleiche mit bekannten Karten
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
      return i + 1; // Rückgabe 1-5
    }
  }
  
  mfrc522.PICC_HaltA();
  return 0; // Unbekannte Karte
}

void startSampleSequence(byte sampleIndex) {
  currentState = GAME_CODE_INPUT;
  sampleIndex--; // Array Index (0-4)
  
  // Entsprechenden Sound abspielen
  mp3Player.play(samples[sampleIndex].soundFile);
  
  // Gelbe LED einschalten
  digitalWrite(YELLOW_LED_PIN, HIGH);
  
  // Display auf CODE setzen
  showCODE();
  
  // LED Strip auf entsprechende Farbe setzen
  switch (sampleIndex) {
    case 0: setLEDStripColor(CRGB::Blue); break;    // BLAU
    case 1: setLEDStripColor(CRGB::Red); break;     // ROT
    case 2: setLEDStripColor(CRGB::Green); break;   // GRÜN
    case 3: setLEDStripColor(CRGB::Yellow); break;  // GELB
    case 4: setLEDStripColor(CRGB::White); break;   // SAMPLE
  }
  
  currentCode = "";
  
  Serial.println("Sample erkannt: " + samples[sampleIndex].name);
  Serial.println("Code eingeben...");
  
  // Debug: Zeige alle möglichen Codes für diese Karte
  Serial.println("== MÖGLICHE CODES FÜR DIESE KARTE ==");
  for (int i = 0; i < 5; i++) {
    bool sameCard = true;
    for (int j = 0; j < 4; j++) {
      if (mfrc522.uid.uidByte[j] != samples[i].uid[j]) {
        sameCard = false;
        break;
      }
    }
    if (sameCard) {
      Serial.print(samples[i].name);
      Serial.print(": ");
      Serial.println(samples[i].code);
    }
  }
  Serial.println("===========================================");
}

void checkCode() {
  int enteredCode = currentCode.toInt();
  
  // Prüfe alle möglichen Codes für die aktuelle Karte
  bool codeCorrect = false;
  int correctSampleIndex = -1;
  
  // Suche nach der Karte mit der aktuellen UID
  for (int i = 0; i < 5; i++) {
    // Prüfe ob es die gleiche UID ist
    bool sameCard = true;
    for (int j = 0; j < 4; j++) {
      if (mfrc522.uid.uidByte[j] != samples[i].uid[j]) {
        sameCard = false;
        break;
      }
    }
    
    // Wenn gleiche Karte und richtiger Code
    if (sameCard && samples[i].code == enteredCode) {
      codeCorrect = true;
      correctSampleIndex = i;
      currentSampleType = i + 1; // Update für Relais-Trigger
      break;
    }
  }
  
  if (codeCorrect) {
    // Richtiger Code gefunden
    Serial.print("Korrekter Code für: ");
    Serial.println(samples[correctSampleIndex].name);
    playSuccessSequence();
  } else {
    // Falscher Code
    Serial.print("Falscher Code! Eingegeben: ");
    Serial.println(enteredCode);
    playErrorSequence();
  }
}

void playSuccessSequence() {
  currentState = GAME_PROCESSING;
  
  // ALLES STARTET SOFORT ZUSAMMEN:
  // MP3 abspielen: "Rechner fährt hoch"
  mp3Player.play(8);
  
  // Display "PASS" anzeigen
  showPASS();
  
  // LED-Laufschrift triggern SOFORT
  digitalWrite(SCROLLTEXT_PIN, HIGH);
  delay(200); // Kurzer Impuls
  digitalWrite(SCROLLTEXT_PIN, LOW);
  Serial.println("LED-Laufschrift getriggert (Pin 13)");
  
  // LED ANIMATION STARTET SOFORT - PARALLEL ZUM SOUND!
  unsigned long animationStart = millis();
  Serial.println("LED Animation SOFORT gestartet - parallel zu Sound!");
  
  // LED Animation Loop - läuft während Sound abgespielt wird
  while ((millis() - animationStart) < 20000) { // 20 Sekunden Animation
    unsigned long elapsed = millis() - animationStart;
    
    // Alle LEDs aus
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    
    if (elapsed >= 4000 && elapsed < 6000) {
      // Von 4-6 Sekunden: LED 0 blinkt blau 16x (~125ms)
      int blinkPhase = (elapsed - 4000) / 125;
      if (blinkPhase < 16 && ((elapsed - 4000) % 250) < 125) {
        leds[0] = CRGB::Blue;
      }
    }
    else if (elapsed >= 8000 && elapsed < 9000) {
      // Von 8-9 Sekunden: LED 1 blinkt grün 2x (~250ms)
      int blinkPhase = (elapsed - 8000) / 500;
      if (blinkPhase < 2 && ((elapsed - 8000) % 1000) < 500) {
        leds[1] = CRGB::Green;
      }
    }
    else if (elapsed >= 9000 && elapsed < 20000) {
      // Von 9-20 Sekunden: LED 1 dauerhaft grün
      leds[1] = CRGB::Green;
    }
    
    FastLED.show();
    delay(50); // Kurze Pause für Stabilität
  }
  
  // Animation beendet - alle LEDs aus
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  Serial.println("LED Animation beendet - alle LEDs aus");
  
  // Entsprechendes Relais aktivieren
  byte relayPin = samples[currentSampleType - 1].relayPin;
  if (relayPin != 0) {
    digitalWrite(relayPin, LOW); // Relais aktivieren
    delay(500);
    digitalWrite(relayPin, HIGH); // Relais deaktivieren
  }
  
  delay(2000);
  
  // Zurück zu HOLD
  resetToHold();
  
  Serial.println("Korrekter Code eingegeben!");
}

void playErrorSequence() {
  currentState = GAME_PROCESSING;
  
  // ALLES STARTET SOFORT ZUSAMMEN:
  // Fehlerton abspielen
  mp3Player.play(9);
  
  // Rote LED und Buzzer SOFORT parallel zum Sound starten
  isBlinking = true;
  blinkCount = 3 * 2; // 3x blinken = 6 Zyklen (an/aus)
  lastBlinkTime = millis();
  ledState = false;
  
  // Buzzer kurz aktivieren - SOFORT
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("Fehlerton + rote LED + Buzzer SOFORT gestartet!");
  
  delay(1000);
  
  // Zurück zu Code Eingabe
  showCODE();
  currentCode = "";
  currentState = GAME_CODE_INPUT;
  
  Serial.println("Falscher Code!");
}

void playFailSequence() {
  // ALLES STARTET SOFORT ZUSAMMEN:
  // Sound: "Kein Gefäß erkannt"
  mp3Player.play(6);
  
  // Rote LED und Buzzer SOFORT parallel zum Sound starten
  isBlinking = true;
  blinkCount = 3 * 2; // 3x blinken = 6 Zyklen (an/aus)
  lastBlinkTime = millis();
  ledState = false;
  
  // Buzzer kurz aktivieren - SOFORT
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Display FAIL anzeigen
  showFAIL();
  
  Serial.println("Fail-Sound + rote LED + Buzzer SOFORT gestartet!");
  
  delay(2000);
  
  // Zurück zu HOLD
  resetToHold();
  
  Serial.println("Kein gültiges Gefäß erkannt!");
}

void resetToHold() {
  currentState = GAME_HOLD;
  currentSampleType = 0;
  currentCode = "";
  
  // LEDs ausschalten
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // LED Strip ausschalten
  setLEDStripColor(CRGB::Black);
  
  // Display auf HOLD
  showHOLD();
  
  Serial.println("Zurück zu HOLD Modus");
}

void startBlinking(int times) {
  isBlinking = true;
  blinkCount = times * 2; // *2 wegen an/aus Zyklen
  lastBlinkTime = millis();
  ledState = false;
}

void startBlinkingWithBuzzer(int times) {
  startBlinking(times);
  // Zusätzlich kurzer Summer-Ton
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
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
      digitalWrite(BUZZER_PIN, LOW); // Summer sicher ausschalten
    }
  }
}

// Taster-Entprellung und Erkennung
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
      return true; // Gültiger Tastendruck erkannt
    }
    buttonPressed = false;
  }
  
  lastButtonState = currentButtonState;
  return false;
}

// Start-Beep beim Drücken des Tasters
void playStartBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100); // Kurzer Beep
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Start-Taster gedrückt - Beep!");
}

// LED-Laufschrift triggern
void triggerScrollText() {
  digitalWrite(SCROLLTEXT_PIN, HIGH);
  delay(200); // Kurzer Impuls
  digitalWrite(SCROLLTEXT_PIN, LOW);
  Serial.println("LED-Laufschrift getriggert (Pin 13)");
}

void setLEDStripColor(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

// Display Funktionen
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

void showPASS() {
  uint8_t data[] = {0x73, 0x77, 0x6d, 0x6d}; // P-A-S-S
  display.setSegments(data);
}

/*
 * HINWEISE:
 * 
 * 1. RFID Karten müssen mit den entsprechenden UIDs programmiert werden
 * 2. MP3 Dateien müssen auf SD-Karte des DFPlayers in der Reihenfolge 001.mp3 bis 009.mp3
 * 3. LED Strip Anzahl in NUM_LEDS anpassen
 * 4. Relais Module können unterschiedliche Logik haben (HIGH/LOW für ON/OFF)
 * 5. Display Segmente können je nach Modell variieren
 * 6. Power-LED (Pin 12) leuchtet dauerhaft zur Stromkontrolle
 * 
 * RFID UID Programmierung:
 * - Verwende Tools wie "RFID Tool" oder "NFC TagInfo" 
 * - Oder programmiere mit separatem Arduino Sketch
 * 
 * Test Funktionen:
 * - Serial Monitor für Debugging verwenden
 * - Einzelne Komponenten separat testen
 */
