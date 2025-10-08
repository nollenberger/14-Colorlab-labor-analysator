# Pinbelegung Arduino Mega - Escape Room Projekt

## Übersicht der verwendeten Komponenten
- Arduino Mega 2560
- RFID Scanner RC522
- 4-Kanal Relais Modul
- 4x4 Matrix Keypad
- 4-Digit 7-Segment Display (TM1637)
- Gelbe LED (Richtig-Anzeige)
- Rote LED (Falsch-Anzeige)
- Power-LED (Stromversorgung-Kontrolle)
- Start Taster (Impulsgeber)
- Summer/Buzzer
- LED-Laufschrift Trigger
- LED-Streifen
- Step-Down Converter
- DFPlayer Mini (MP3-Player)
- Display (über USB, extern gesteuert)

## Detaillierte Pinbelegung

### RFID Scanner RC522
- **VCC**: 3.3V (Arduino)
- **RST**: Pin 9
- **GND**: GND (Arduino)
- **IRQ**: Nicht angeschlossen
- **MISO**: Pin 50 (Arduino Mega SPI)
- **MOSI**: Pin 51 (Arduino Mega SPI)
- **SCK**: Pin 52 (Arduino Mega SPI)
- **SDA/SS**: Pin 53

### 4-Kanal Relais Modul
- **VCC**: 5V (Arduino)
- **GND**: GND (Arduino)
- **IN1**: Pin 22 (Video Trigger 1 - BLAU)
- **IN2**: Pin 24 (Video Trigger 2 - ROT)
- **IN3**: Pin 26 (Video Trigger 3 - GRÜN)
- **IN4**: Pin 28 (Video Trigger 4 - GELB)

### 4x4 Matrix Keypad
- **R1**: Pin 30
- **R2**: Pin 32
- **R3**: Pin 34
- **R4**: Pin 36
- **C1**: Pin 38
- **C2**: Pin 40
- **C3**: Pin 42
- **C4**: Pin 44

### 4-Digit 7-Segment Display (TM1637)
- **VCC**: 5V (Arduino)
- **GND**: GND (Arduino)
- **CLK**: Pin 2
- **DIO**: Pin 3

### LEDs
- **Gelbe LED** (Richtig): Pin 8 (+ Vorwiderstand 220Ω)
- **Rote LED** (Falsch): Pin 7 (+ Vorwiderstand 220Ω)
- **Power-LED** (Stromkontrolle): Pin 12 (+ Vorwiderstand 220Ω)

### Summer/Buzzer
- **Buzzer**: Pin 5 (aktiver Summer/Piezo-Summer)
- **VCC**: 5V (Arduino)
- **GND**: GND (Arduino)

### LED-Laufschrift Trigger
- **Trigger**: Pin 13 (digitaler Ausgang)
- **Funktion**: Kurzer Impuls (200ms) zum Triggern externer LED-Laufschrift Hardware

### Start Taster
- **Taster**: Pin 4 (mit Pull-up Widerstand 10kΩ oder INPUT_PULLUP)
- **GND**: GND (Arduino)
- **Funktion**: Kurzer Impuls beim Drücken (nicht dauerhaft gedrückt halten)

### LED-Streifen
- **Data**: Pin 6
- **VCC**: 5V (externe Stromversorgung empfohlen)
- **GND**: GND (Arduino + externe Stromversorgung)

### DFPlayer Mini (MP3)
- **VCC**: 5V (Arduino)
- **GND**: GND (Arduino)
- **RX**: Pin 10 (Arduino TX)
- **TX**: Pin 11 (Arduino RX)

### Step-Down Converter
- **Input**: Externe Stromversorgung (12V empfohlen)
- **Output**: 5V für Arduino und Komponenten

## Stromversorgung
- **Arduino Mega**: Über Step-Down Converter (5V)
- **Externe Komponenten**: 5V vom Step-Down Converter
- **LED-Streifen**: Separate 5V Versorgung empfohlen bei >10 LEDs

## Wichtige Hinweise
- Alle GND-Verbindungen müssen gemeinsam verbunden werden
- RFID RC522 benötigt 3.3V, nicht 5V!
- Bei längeren LED-Streifen separate Stromversorgung verwenden
- Pull-up Widerstände für Button verwenden
- Vorwiderstände für LEDs nicht vergessen

## Kabelfarben Empfehlung
- **Rot**: VCC/+5V
- **Schwarz**: GND
- **Gelb**: Signalleitungen
- **Grün**: SPI-Leitungen
- **Blau**: I2C-Leitungen
- **Orange**: Analog-Eingänge
