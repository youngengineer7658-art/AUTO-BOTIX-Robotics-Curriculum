/*
==================================================
Day 45 | Practical-90: Plan and Test Modular System
Board  : Young Engineer Board
Hardware Pinout:
  - Ultrasonic Sensor   : Trig D5, Echo A3
  - IR Sensor (Analog)  : Pin A0
  - Button Ladder Pin   : Pin A6 (Calibrated Values)
  - Display             : 0.96" I2C OLED (U8g2 - RAM Safe)

Modular Architecture:
  - Module 1: Sense (Non-blocking sensor acquisition)
  - Module 2: Control/Logic (State machine & obstacle decision engine)
  - Module 3: Actuate/UI (RAM-safe OLED pages & button-selectable screens)

Navigation via Buttons (Pin A6):
  - Select (925-938) : Switch Module View (ALL SENSORS -> LOGIC STATE)
  - Back   (728-740) : Reset System / Fault Clear
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_PIN   A0
#define BTN_PIN  A6

// Calibrated Ladder Ranges
#define BTN_SEL_MIN  925
#define BTN_SEL_MAX  938
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

enum Button { BTN_NONE, BTN_SELECT, BTN_BACK };

Button readButton() {
  int val = analogRead(BTN_PIN);
  if (val >= BTN_SEL_MIN  && val <= BTN_SEL_MAX)  return BTN_SELECT;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

// RAM-Safe U8g2 Constructor
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// UI Screen Modes
enum ViewPage { PAGE_SENSORS, PAGE_LOGIC };
ViewPage currentPage = PAGE_SENSORS;

// Robot Decision States
enum DecisionState { STATE_FORWARD, STATE_OBSTACLE_NEAR, STATE_STOP_BLOCKED };
DecisionState currentDecision = STATE_FORWARD;

// Telemetry & Metrics
int distanceCm = 0;
int irRawVal = 0;
bool irDetected = false;

unsigned long lastSenseTime = 0;
const unsigned long SENSE_INTERVAL = 150; // Sense task every 150ms

Button lastBtn = BTN_NONE;
unsigned long lastDebounce = 0;

// ================= MODULE 1: SENSE =================
void taskSense() {
  // Ultrasonic pulse (Trig: D5, Echo: A3)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0 || duration > 25000UL) {
    distanceCm = -1;
  } else {
    distanceCm = (int)((duration * 0.034) / 2);
  }

  // IR Sensor on A0
  irRawVal = analogRead(IR_PIN);
  irDetected = (irRawVal < 500);
}

// ================= MODULE 2: LOGIC =================
void taskLogic() {
  if (irDetected || (distanceCm > 0 && distanceCm < 15)) {
    currentDecision = STATE_STOP_BLOCKED;
  } else if (distanceCm >= 15 && distanceCm <= 30) {
    currentDecision = STATE_OBSTACLE_NEAR;
  } else {
    currentDecision = STATE_FORWARD;
  }
}

// ================= MODULE 3: ACTUATE / UI =================
void taskDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "MODULAR SYSTEM: P-90");
    u8g2.drawHLine(0, 15, 128);

    if (currentPage == PAGE_SENSORS) {
      u8g2.drawStr(5, 29, "[VIEW: SENSORS]");

      u8g2.drawStr(5, 44, "US DIST :");
      if (distanceCm == -1) {
        u8g2.drawStr(65, 44, "-- cm");
      } else {
        char dBuf[6]; itoa(distanceCm, dBuf, 10);
        u8g2.drawStr(65, 44, dBuf);
        u8g2.drawStr(95, 44, "cm");
      }

      u8g2.drawStr(5, 59, "IR (A0) :");
      u8g2.drawStr(65, 59, irDetected ? "DETECTED" : "CLEAR");
    } 
    else {
      u8g2.drawStr(5, 29, "[VIEW: DECISION]");

      u8g2.drawStr(5, 44, "ACTION :");
      switch (currentDecision) {
        case STATE_FORWARD:
          u8g2.drawStr(60, 44, "GO FORWARD");
          break;
        case STATE_OBSTACLE_NEAR:
          u8g2.drawStr(60, 44, "SLOW / STEER");
          break;
        case STATE_STOP_BLOCKED:
          u8g2.drawStr(60, 44, "BRAKE / STOP");
          break;
      }

      u8g2.drawStr(5, 59, "SEL:PAGE | BACK:RST");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 45 | PRACTICAL-90: MODULAR SYSTEM  "));
  Serial.println(F("========================================"));
  Serial.println(F("SEL: Toggle Page | BACK: Reset State"));
}

void loop() {
  // Input Handling
  Button btn = readButton();
  if (btn != lastBtn && (millis() - lastDebounce > 200)) {
    lastDebounce = millis();
    if (btn == BTN_SELECT) {
      currentPage = (currentPage == PAGE_SENSORS) ? PAGE_LOGIC : PAGE_SENSORS;
      Serial.print(F("[UI] Switched Page to: "));
      Serial.println(currentPage == PAGE_SENSORS ? F("SENSORS") : F("LOGIC"));
    } else if (btn == BTN_BACK) {
      currentDecision = STATE_FORWARD;
      Serial.println(F("[SYSTEM] State Reset Triggered"));
    }
  }
  lastBtn = btn;

  // Task Scheduler (Non-blocking)
  unsigned long currentMillis = millis();
  if (currentMillis - lastSenseTime >= SENSE_INTERVAL) {
    lastSenseTime = currentMillis;

    taskSense();    // 1. Gather sensor inputs
    taskLogic();    // 2. Compute robot navigation decisions
    taskDisplay();  // 3. Render telemetry & decisions to OLED

    // Telemetry serial log
    Serial.print(F("US: "));
    Serial.print(distanceCm);
    Serial.print(F("cm | IR: "));
    Serial.print(irDetected ? F("OBJ") : F("CLEAR"));
    Serial.print(F(" | Action: "));
    if (currentDecision == STATE_FORWARD) Serial.println(F("FORWARD"));
    else if (currentDecision == STATE_OBSTACLE_NEAR) Serial.println(F("SLOW"));
    else Serial.println(F("STOP"));
  }
}
