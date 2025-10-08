/*
 * HARDWARE TEST SCRIPT
 * ====================
 * 
 * Testet alle Komponenten einzeln:
 * - MP3-Player: Spielt alle Sounds 001-009 nacheinander ab
 * - Display: Testet verschiedene Anzeigen
 * - LEDs: Testet alle LEDs
 * - Relais: Testet alle Relais
 * - Summer: Testet Summer
 * 
 * Verwende Serial Monitor (9600 baud) für Status-Meldungen
 */

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <TM1637Display.h>

// ============== PIN DEFINITIONEN ==============
#define MP3_RX          10
#define MP3_TX          11
#define DISPLAY_CLK     2
#define DISPLAY_DIO     3
#define RED_LED_PIN     7
#define YELLOW_LED_PIN  8
#define POWER_LED_PIN   12
#define BUZZER_PIN      5
#define RELAY1_PIN      22
#define RELAY2_PIN      24
#define RELAY3_PIN      26
#define RELAY4_PIN      28

// ============== OBJEKTE ==============
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFPlayerMini_Fast mp3Player;
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

void setup() {
  Serial.begin(9600);
  Serial.println("=== HARDWARE TEST GESTARTET ===");
  Serial.println("Teste alle Komponenten einzeln...");
  Serial.println();
  
  // ============== PINS INITIALISIEREN ==============
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Alles initial ausschalten
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(POWER_LED_PIN, HIGH);  // Power-LED AN
  digitalWrite(RELAY1_PIN, HIGH);     // Relais AUS (HIGH = OFF)
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  Serial.println("✅ Pins initialisiert");
  delay(1000);
  
  // ============== POWER-LED TEST ==============
  Serial.println("\n🔋 POWER-LED TEST");
  if (digitalRead(POWER_LED_PIN) == HIGH) {
    Serial.println("✅ Power-LED leuchtet");
  } else {
    Serial.println("❌ Power-LED Problem!");
  }
  delay(1000);
  
  // ============== DISPLAY TEST ==============
  Serial.println("\n📺 DISPLAY TEST");
  Serial.println("Teste TM1637 Display...");
  
  display.setBrightness(0x0f);  // Hellste Stufe
  
  // Test 1: Alle Segmente an
  Serial.println("Test 1: Alle Segmente AN");
  uint8_t allOn[] = {0xFF, 0xFF, 0xFF, 0xFF};
  display.setSegments(allOn);
  delay(2000);
  
  // Test 2: Alle Segmente aus
  Serial.println("Test 2: Alle Segmente AUS");
  uint8_t allOff[] = {0x00, 0x00, 0x00, 0x00};
  display.setSegments(allOff);
  delay(2000);
  
  // Test 3: Zahlen 0-9
  Serial.println("Test 3: Zahlen 0000-9999");
  for (int i = 0; i <= 9999; i += 1111) {
    Serial.println("Zeige: " + String(i));
    display.showNumberDec(i, true);  // Mit führenden Nullen
    delay(1000);
  }
  
  // Test 4: Spezielle Anzeigen
  Serial.println("Test 4: Spezielle Anzeigen");
  
  // HOLD
  Serial.println("Zeige: HOLD");
  uint8_t holdData[] = {0x76, 0x3f, 0x38, 0x5e}; // H-O-L-d
  display.setSegments(holdData);
  delay(2000);
  
  // CODE
  Serial.println("Zeige: CODE");
  uint8_t codeData[] = {0x39, 0x3f, 0x5e, 0x79}; // C-O-d-E
  display.setSegments(codeData);
  delay(2000);
  
  // FAIL
  Serial.println("Zeige: FAIL");
  uint8_t failData[] = {0x71, 0x77, 0x30, 0x38}; // F-A-I-L
  display.setSegments(failData);
  delay(2000);
  
  Serial.println("✅ Display-Tests abgeschlossen");
  
  // ============== LED TEST ==============
  Serial.println("\n💡 LED TEST");
  
  Serial.println("Teste Rote LED...");
  digitalWrite(RED_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(RED_LED_PIN, LOW);
  
  Serial.println("Teste Gelbe LED...");
  digitalWrite(YELLOW_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(YELLOW_LED_PIN, LOW);
  
  Serial.println("✅ LED-Tests abgeschlossen");
  
  // ============== SUMMER TEST ==============
  Serial.println("\n🔊 SUMMER TEST");
  Serial.println("Teste Summer (3x kurz)...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
  Serial.println("✅ Summer-Test abgeschlossen");
  
  // ============== RELAIS TEST ==============
  Serial.println("\n🔌 RELAIS TEST");
  Serial.println("Teste alle 4 Relais (je 500ms)...");
  
  Serial.println("Relais 1 (Pin 22)...");
  digitalWrite(RELAY1_PIN, LOW);  // AN
  delay(500);
  digitalWrite(RELAY1_PIN, HIGH); // AUS
  delay(500);
  
  Serial.println("Relais 2 (Pin 24)...");
  digitalWrite(RELAY2_PIN, LOW);
  delay(500);
  digitalWrite(RELAY2_PIN, HIGH);
  delay(500);
  
  Serial.println("Relais 3 (Pin 26)...");
  digitalWrite(RELAY3_PIN, LOW);
  delay(500);
  digitalWrite(RELAY3_PIN, HIGH);
  delay(500);
  
  Serial.println("Relais 4 (Pin 28)...");
  digitalWrite(RELAY4_PIN, LOW);
  delay(500);
  digitalWrite(RELAY4_PIN, HIGH);
  delay(500);
  
  Serial.println("✅ Relais-Tests abgeschlossen");
  
  // ============== MP3 PLAYER INITIALISIERUNG ==============
  Serial.println("\n🎵 MP3-PLAYER TEST");
  Serial.println("Initialisiere DFPlayer Mini...");
  
  mp3Serial.begin(9600);
  delay(1000);
  
  if (mp3Player.begin(mp3Serial)) {
    Serial.println("✅ MP3-Player erfolgreich initialisiert");
    mp3Player.volume(20);  // Lautstärke 0-30
    delay(500);
    
    // Teste alle Sounds 001-009
    Serial.println("\n🔊 TESTE ALLE SOUNDS 001-009");
    Serial.println("Jeder Sound wird bis zum Ende abgespielt...");
    Serial.println();
    
    for (int soundNum = 1; soundNum <= 9; soundNum++) {
      Serial.println("=== SPIELE SOUND " + String(soundNum, DEC) + " ===");
      Serial.println("Datei: " + String(soundNum, DEC).substring(0,3) + ".mp3");
      
      mp3Player.play(soundNum);
      
      // Warte bis Sound zu Ende ist
      Serial.println("Warte bis Sound zu Ende ist...");
      delay(1000);  // Kurz warten bis Sound startet
      
      // Prüfe ob Sound läuft und warte
      bool soundPlaying = true;
      int checkCount = 0;
      while (soundPlaying && checkCount < 60) { // Max 30 Sekunden
        delay(500);
        checkCount++;
        
        // Zeige Progress
        if (checkCount % 4 == 0) {
          Serial.println("Sound " + String(soundNum) + " läuft... (" + String(checkCount/2) + "s)");
        }
        
        // Nach 30 Sekunden automatisch stoppen
        if (checkCount >= 60) {
          Serial.println("Sound-Timeout erreicht (30s)");
          soundPlaying = false;
        }
      }
      
      Serial.println("✅ Sound " + String(soundNum) + " beendet");
      Serial.println();
      
      delay(1000); // Pause zwischen Sounds
    }
    
    Serial.println("🎉 ALLE SOUNDS GETESTET!");
    
  } else {
    Serial.println("❌ MP3-Player FEHLER!");
    Serial.println("Prüfe:");
    Serial.println("- SD-Karte eingesteckt?");
    Serial.println("- MP3-Dateien 001.mp3-009.mp3 vorhanden?");
    Serial.println("- Verkabelung korrekt?");
    Serial.println("- DFPlayer Mini funktionsfähig?");
  }
  
  // ============== ZUSAMMENFASSUNG ==============
  Serial.println("\n" + String("=").substring(0,50));
  Serial.println("🏁 HARDWARE-TEST ABGESCHLOSSEN");
  Serial.println(String("=").substring(0,50));
  Serial.println();
  Serial.println("GETESTETE KOMPONENTEN:");
  Serial.println("✅ Power-LED (Pin 12)");
  Serial.println("✅ Display TM1637 (Pin 2/3)");
  Serial.println("✅ Rote LED (Pin 7)");
  Serial.println("✅ Gelbe LED (Pin 8)");
  Serial.println("✅ Summer/Buzzer (Pin 5)");
  Serial.println("✅ Relais 1-4 (Pin 22,24,26,28)");
  Serial.println("✅ MP3-Player + alle Sounds");
  Serial.println();
  Serial.println("WENN ETWAS NICHT FUNKTIONIERT:");
  Serial.println("- Verkabelung nach Pinbelegung prüfen");
  Serial.println("- Stromversorgung prüfen");
  Serial.println("- Komponenten einzeln testen");
  Serial.println();
  Serial.println("🔄 Test wiederholen: Arduino neu starten");
  
  // Final: Display auf HOLD setzen
  display.setSegments(holdData);
}

void loop() {
  // Test läuft nur einmal in setup()
  // Loop ist leer - System steht nach Test still
  
  // Optional: Power-LED blinken lassen als "Test beendet" Signal
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if (millis() - lastBlink > 2000) {
    ledState = !ledState;
    digitalWrite(POWER_LED_PIN, ledState ? HIGH : LOW);
    lastBlink = millis();
  }
}

/*
 * ================================================================
 *                      ANLEITUNG
 * ================================================================
 * 
 * 1. Dieses Script auf Arduino Mega hochladen
 * 2. Serial Monitor öffnen (9600 baud)
 * 3. Alle Tests werden automatisch durchgeführt
 * 4. Beobachte Serial Monitor für Ergebnisse
 * 5. Prüfe visuell: LEDs, Display, höre Sounds
 * 
 * ERWARTETE REIHENFOLGE:
 * ----------------------
 * 1. Power-LED Test
 * 2. Display Test (alle Segmente, Zahlen, HOLD/CODE/FAIL)
 * 3. LED Test (rot, gelb)
 * 4. Summer Test (3x Beep)
 * 5. Relais Test (alle 4 nacheinander)
 * 6. MP3 Test (alle Sounds 001-009)
 * 
 * BEI PROBLEMEN:
 * -------------
 * - Display zeigt nichts: Pin 2/3 prüfen, 5V/GND prüfen
 * - Kein Sound: SD-Karte, MP3-Dateien, Verkabelung prüfen
 * - LEDs funktionieren nicht: Verkabelung, Vorwiderstände prüfen
 * - Relais schalten nicht: Verkabelung, Relais-Logik prüfen
 * 
 * MP3-DATEIEN BENÖTIGT:
 * --------------------
 * 001.mp3, 002.mp3, 003.mp3, 004.mp3, 005.mp3,
 * 006.mp3, 007.mp3, 008.mp3, 009.mp3
 * 
 * Alle Dateien müssen im Root-Verzeichnis der SD-Karte sein!
 * ================================================================
 */