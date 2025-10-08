/*
 * FASTLED WS2812B TEST SCRIPT
 * ===========================
 * 
 * Testet WS2812B LED Strip auf verschiedenen Pins
 * 
 * Hardware:
 * - Arduino Mega 2560
 * - WS2812B Strip mit 2 LEDs
 * - 5V Stromversorgung
 * 
 * Verkabelung:
 * - +5V → Arduino 5V
 * - GND → Arduino GND
 * - Data → Arduino PIN (wird getestet)
 * 
 * Version: 1.0
 * Datum: 2025-10-06
 */

#include <FastLED.h>

// ============== TEST KONFIGURATION ==============
#define NUM_LEDS    2         // 2 LEDs im Strip
#define LED_TYPE    WS2812B   // LED Typ
#define COLOR_ORDER GRB       // Farbreihenfolge

// Test verschiedene Pins
#define TEST_PIN_1    6       // Ursprünglicher Pin
#define TEST_PIN_2    14      // Neuer Pin
#define TEST_PIN_3    15      // Alternative
#define TEST_PIN_4    16      // Alternative

CRGB leds[NUM_LEDS];
int currentTestPin = TEST_PIN_14;
int testPhase = 0;

// ============== SETUP FUNKTION ==============
void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(100); }
  
  Serial.println("=== FASTLED WS2812B TEST ===");
  Serial.println("Testet 2 LEDs auf verschiedenen Pins");
  Serial.println("===============================");
  
  startTestPin(TEST_PIN_1);
}

// ============== HAUPT LOOP ==============
void loop() {
  
  // Test Phase 1: Alle LEDs rot
  if (testPhase == 0) {
    Serial.println("Phase 1: Alle LEDs ROT");
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Red;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 2: Alle LEDs grün
  else if (testPhase == 1) {
    Serial.println("Phase 2: Alle LEDs GRÜN");
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 3: Alle LEDs blau
  else if (testPhase == 2) {
    Serial.println("Phase 3: Alle LEDs BLAU");
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Blue;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 4: Alle LEDs weiß
  else if (testPhase == 3) {
    Serial.println("Phase 4: Alle LEDs WEISS");
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 5: Alle LEDs orange
  else if (testPhase == 4) {
    Serial.println("Phase 5: Alle LEDs ORANGE");
    leds[0] = CRGB::Orange;
    leds[1] = CRGB::Orange;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 6: LED 0 orange, LED 1 grün
  else if (testPhase == 5) {
    Serial.println("Phase 6: LED 0 = ORANGE, LED 1 = GRÜN");
    leds[0] = CRGB::Orange;
    leds[1] = CRGB::Green;
    FastLED.show();
    delay(2000);
    testPhase++;
  }
  
  // Test Phase 7: Blinken
  else if (testPhase == 6) {
    Serial.println("Phase 7: LEDs blinken (5x)");
    for (int i = 0; i < 5; i++) {
      leds[0] = CRGB::Red;
      leds[1] = CRGB::Blue;
      FastLED.show();
      delay(300);
      
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Black;
      FastLED.show();
      delay(300);
    }
    testPhase++;
  }
  
  // Test Phase 8: Laufende LEDs
  else if (testPhase == 7) {
    Serial.println("Phase 8: Laufende LEDs (10x)");
    for (int i = 0; i < 10; i++) {
      // Nur LED 0 an
      leds[0] = CRGB::Cyan;
      leds[1] = CRGB::Black;
      FastLED.show();
      delay(500);
      
      // Nur LED 1 an
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Magenta;
      FastLED.show();
      delay(500);
    }
    testPhase++;
  }
  
  // Test abgeschlossen - nächster Pin
  else {
    Serial.println("===============================");
    Serial.println("Test für PIN " + String(currentTestPin) + " abgeschlossen!");
    Serial.println("===============================");
    
    // LEDs ausschalten
    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    FastLED.show();
    
    delay(3000);
    
    // Nächster Pin
    if (currentTestPin == TEST_PIN_1) {
      startTestPin(TEST_PIN_2);
    } else if (currentTestPin == TEST_PIN_2) {
      startTestPin(TEST_PIN_3);
    } else if (currentTestPin == TEST_PIN_3) {
      startTestPin(TEST_PIN_4);
    } else {
      Serial.println("===============================");
      Serial.println("ALLE PINS GETESTET!");
      Serial.println("Falls keine LEDs leuchteten:");
      Serial.println("1. Stromversorgung prüfen (5V, GND)");
      Serial.println("2. Data-Pin Verkabelung prüfen");
      Serial.println("3. LED-Strip defekt?");
      Serial.println("4. Falsche LED-Reihenfolge?");
      Serial.println("===============================");
      delay(10000);
      startTestPin(TEST_PIN_1);  // Von vorne beginnen
    }
  }
}

// ============== PIN TEST STARTEN ==============
void startTestPin(int pin) {
  currentTestPin = pin;
  testPhase = 0;
  
  Serial.println("===============================");
  Serial.println("TESTE PIN " + String(pin));
  Serial.println("Verkabelung:");
  Serial.println("- +5V → Arduino 5V");
  Serial.println("- GND → Arduino GND");
  Serial.println("- Data → Arduino PIN " + String(pin));
  Serial.println("===============================");
  
  // FastLED für neuen Pin initialisieren
  FastLED.clear();
  
  if (pin == TEST_PIN_1) {
    FastLED.addLeds<LED_TYPE, TEST_PIN_1, COLOR_ORDER>(leds, NUM_LEDS);
  } else if (pin == TEST_PIN_2) {
    FastLED.addLeds<LED_TYPE, TEST_PIN_2, COLOR_ORDER>(leds, NUM_LEDS);
  } else if (pin == TEST_PIN_3) {
    FastLED.addLeds<LED_TYPE, TEST_PIN_3, COLOR_ORDER>(leds, NUM_LEDS);
  } else if (pin == TEST_PIN_4) {
    FastLED.addLeds<LED_TYPE, TEST_PIN_4, COLOR_ORDER>(leds, NUM_LEDS);
  }
  
  FastLED.setBrightness(80);  // Mittlere Helligkeit
  
  // Alle LEDs ausschalten
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  FastLED.show();
  
  Serial.println("FastLED initialisiert für PIN " + String(pin));
  Serial.println("Starte Test...");
  delay(1000);
}

/*
 * ERWARTETES VERHALTEN:
 * ====================
 * 
 * Wenn die LEDs funktionieren:
 * - Phase 1: Beide LEDs rot
 * - Phase 2: Beide LEDs grün  
 * - Phase 3: Beide LEDs blau
 * - Phase 4: Beide LEDs weiß
 * - Phase 5: Beide LEDs orange
 * - Phase 6: LED 0 orange, LED 1 grün
 * - Phase 7: LEDs blinken abwechselnd
 * - Phase 8: LEDs laufen hin und her
 * 
 * FEHLERBEHEBUNG:
 * ===============
 * 
 * Falls nichts leuchtet:
 * 1. Stromversorgung: 5V und GND richtig angeschlossen?
 * 2. Data-Pin: Kabel fest? Richtiger Pin?
 * 3. LED-Strip: Ist er intakt? Richtige Richtung?
 * 4. Verkabelung: +5V, GND, Data - in dieser Reihenfolge?
 * 
 * Falls nur eine LED leuchtet:
 * - LED 1 defekt oder schlechte Verbindung
 * - Data-Signal kommt nicht zur zweiten LED
 * 
 * Falls falsche Farben:
 * - COLOR_ORDER ändern (RGB statt GRB)
 */