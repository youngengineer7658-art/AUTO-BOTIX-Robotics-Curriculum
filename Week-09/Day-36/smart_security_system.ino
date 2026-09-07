/*
==================================================
Project : Smart Security System with PIN Lock (1234)
Board   : Young Engineer Board / Auto-Botix
Hardware: Ultrasonic (D5, A3) + Keypad (A6) + 
          RGB NeoPixel (D11) + Buzzer (D12) + OLED (U8g2)

Keypad Numbers:
  UP     = 1
  DOWN   = 2
  BACK   = 3
  SELECT = 4 (Also used to ARM when unlocked)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

// 1. PIN DEFINITIONS
#define TRIG_PIN    5
#define ECHO_PIN    A3
#define RGB_PIN     11
#define NUM_LEDS    8
#define BUZZER_PIN  12
#define BUTTON_PIN  A6

// 2. HARDWARE OBJECTS (128 bytes static RAM usage)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel rgb(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// 3. CALIBRATED ADC KEYPAD THRESHOLDS
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945

// 4. SYSTEM STATES & PIN VARIABLES
enum SystemState { UNLOCKED, ARMED, INTRUDER_ALERT };
SystemState currentState = UNLOCKED;

const int TRIGGER_DISTANCE = 30; // 30 cm perimeter threshold
int distance = 999;
int lastButtonState = 0;

// PIN Authentication Variables
const char correctPIN[] = "1234";
char enteredPIN[5] = "";
byte pinIndex = 0;
const char* lockMessage = "ENTER PIN";

// Non-blocking Timers
unsigned long lastSensorTime = 0;
unsigned long lastBuzzerToggle = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastSerialTime = 0;
bool buzzerActive = false;

//-------------------------------------------
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    rgb.setPixelColor(i, rgb.Color(r, g, b));
  }
  rgb.show();
}

void rgbOff() {
  rgb.clear();
  rgb.show();
}

void shortBeep(int duration = 80) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

//-------------------------------------------
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

//-------------------------------------------
int readADCButton() {
  int val = analogRead(BUTTON_PIN);
  if (val >= BTN_UP_MIN && val <= BTN_UP_MAX)         return 1; // UP     -> '1'
  if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX)     return 2; // DOWN   -> '2'
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX)     return 3; // BACK   -> '3'
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) return 4; // SELECT -> '4'
  return 0;
}

//-------------------------------------------
void armSystem() {
  currentState = ARMED;
  pinIndex = 0;
  enteredPIN[0] = '\0';
  lockMessage = "ARMED: WATCHING";
  setRGB(0, 0, 255); // Blue: Armed & Guarding
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println(F("[SYSTEM] ARMED - PERIMETER ACTIVE"));
}

void unlockSystem() {
  currentState = UNLOCKED;
  pinIndex = 0;
  enteredPIN[0] = '\0';
  lockMessage = "OPEN: SAFE";
  setRGB(0, 255, 0); // Green: Unlocked & Safe
  digitalWrite(BUZZER_PIN, LOW);
  
  // Success Beeps
  shortBeep(60);
  delay(60);
  shortBeep(80);
  
  Serial.println(F("[ACCESS] GRANTED! LOCK OPENED"));
}

void triggerAlarm() {
  currentState = INTRUDER_ALERT;
  lockMessage = "! BREACHED !";
  Serial.println(F("[ALERT] INTRUSION! ENTER PIN '1234' TO STOP"));
}

//-------------------------------------------
void processKey(char key) {
  shortBeep(40); // Feedback key beep
  
  if (pinIndex < 4) {
    enteredPIN[pinIndex] = key;
    pinIndex++;
    enteredPIN[pinIndex] = '\0';
  }

  Serial.print(F("[KEYPAD] Entered: "));
  Serial.println(enteredPIN);

  // When 4 digits are completed, verify PIN
  if (pinIndex == 4) {
    if (strcmp(enteredPIN, correctPIN) == 0) {
      unlockSystem();
    } else {
      // Wrong PIN Alert
      Serial.println(F("[ACCESS] DENIED! WRONG PIN"));
      lockMessage = "WRONG PIN!";
      setRGB(255, 0, 0);
      
      // Error long beep
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      
      // Reset PIN buffer
      pinIndex = 0;
      enteredPIN[0] = '\0';
      
      if (currentState == ARMED) {
        setRGB(0, 0, 255);
        lockMessage = "ARMED: WATCHING";
      }
    }
  }
}

//-------------------------------------------
void updateOLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(12, 10, "SECURITY LOCK");
    u8g2.drawHLine(0, 13, 128);

    // Status / Message line
    u8g2.drawStr(5, 27, "STATUS:");
    u8g2.drawStr(55, 27, lockMessage);

    // PIN Display with masking (e.g. * * - -)
    u8g2.drawStr(5, 43, "PIN   :");
    char pinDisplay[10] = "";
    for (byte i = 0; i < 4; i++) {
      if (i < pinIndex) strcat(pinDisplay, "* ");
      else strcat(pinDisplay, "_ ");
    }
    u8g2.drawStr(55, 43, pinDisplay);

    // Live Range or Instruction
    u8g2.drawStr(5, 59, "DIST  :");
    if (distance >= 400 || distance <= 0 || distance == 999) {
      u8g2.drawStr(55, 59, "--");
    } else {
      char buf[8];
      itoa(distance, buf, 10);
      u8g2.drawStr(55, 59, buf);
      u8g2.drawStr(85, 59, "cm");
    }
  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  u8g2.begin();
  rgb.begin();
  unlockSystem();

  Serial.println(F("========================================"));
  Serial.println(F("  SMART SECURITY WITH 4-DIGIT PIN LOCK  "));
  Serial.println(F("========================================"));
  Serial.println(F("PIN CODE : 1 2 3 4"));
  Serial.println(F("UP=1 | DOWN=2 | BACK=3 | SELECT=4"));
  Serial.println(F("Press 4 (SELECT) while unlocked to ARM"));

  updateOLED();
}

//-------------------------------------------
void loop() {
  // 1. HARDWARE KEYPAD INPUT (A6)
  int btn = readADCButton();
  if (btn != 0 && lastButtonState == 0) {
    if (currentState == UNLOCKED && btn == 4 && pinIndex == 0) {
      // Direct ARM action using button 4 if no PIN input is ongoing
      armSystem();
    } else {
      char pressedDigit = '0' + btn; // 1 -> '1', 2 -> '2', 3 -> '3', 4 -> '4'
      processKey(pressedDigit);
    }
    updateOLED();
  }
  lastButtonState = btn;

  // 2. ULTRASONIC INTRUSION DETECTION (Every 100ms)
  if (millis() - lastSensorTime >= 100) {
    lastSensorTime = millis();
    distance = readDistance();

    if (currentState == ARMED) {
      if (distance > 0 && distance <= TRIGGER_DISTANCE) {
        triggerAlarm();
      }
    }
  }

  // 3. NON-BLOCKING ALARM SIREN & STROBE (Active during intrusion)
  if (currentState == INTRUDER_ALERT) {
    if (millis() - lastBuzzerToggle >= 120) {
      lastBuzzerToggle = millis();
      buzzerActive = !buzzerActive;

      digitalWrite(BUZZER_PIN, buzzerActive ? HIGH : LOW);
      if (buzzerActive) setRGB(255, 0, 0); // Flashing Red
      else rgbOff();
    }
  }

  // 4. PERIODIC OLED DASHBOARD REFRESH (Every 200ms)
  if (millis() - lastDisplayTime >= 200) {
    lastDisplayTime = millis();
    updateOLED();
  }

  // 5. SERIAL MONITOR TELEMETRY (Every 1 Second)
  if (millis() - lastSerialTime >= 1000) {
    lastSerialTime = millis();
    Serial.print(F("LOCK | Status: "));
    if (currentState == UNLOCKED)        Serial.print(F("UNLOCKED (OPEN) "));
    else if (currentState == ARMED)      Serial.print(F("ARMED (GUARDING)"));
    else                                 Serial.print(F("! INTRUSION !   "));
    Serial.print(F(" | Entered: "));
    Serial.print(pinIndex);
    Serial.print(F(" digits | Dist: "));
    if (distance >= 400 || distance <= 0 || distance == 999) Serial.println(F("--"));
    else { Serial.print(distance); Serial.println(F(" cm")); }
  }
}