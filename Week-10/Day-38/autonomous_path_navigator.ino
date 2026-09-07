/*
==================================================
Day 42 | Practical-83: Run Pump for Fixed Duration
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Ladder Pin : A6 (Calibrated Ranges)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Calibrated Button Values (A6):
  - Select (925-938) : Trigger Fixed 5-Second Watering Burst
  - Back   (728-740) : Emergency Cutoff (Instant Stop)

Logic:
  - Non-blocking millis() countdown timer
  - Auto-shuts off pump after 5000 ms
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Button Ranges
#define BTN_SEL_MIN  925
#define BTN_SEL_MAX  938
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

enum Button { BTN_NONE, BTN_SELECT, BTN_BACK };

Button readButton() {
  int val = analogRead(BUTTONS_PIN);
  if (val >= BTN_SEL_MIN  && val <= BTN_SEL_MAX)  return BTN_SELECT;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

// RAM-Safe OLED Constructor (128 bytes buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Timing Parameters
const unsigned long RUN_DURATION_MS = 5000; // 5 seconds burst
unsigned long pumpStartTime = 0;
bool isPumpRunning = false;

Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void updateDisplay(int secondsLeft) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-83: TIMER");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 34, "DURATION: 5 SEC");

    u8g2.drawStr(5, 50, "PUMP    :");
    if (isPumpRunning) {
      u8g2.drawStr(65, 50, "ON");
      
      u8g2.drawStr(5, 62, "TIME LEFT:");
      char secBuf[4];
      itoa(secondsLeft, secBuf, 10);
      u8g2.drawStr(75, 62, secBuf);
      u8g2.drawStr(90, 62, "s");
    } else {
      u8g2.drawStr(65, 50, "IDLE");
      u8g2.drawStr(5, 62, "PRESS SELECT TO RUN");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);
  digitalWrite(PUMP_PIN, LOW); // Initial safe state

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 42 | PRACTICAL-83: FIXED DURATION  "));
  Serial.println(F("========================================"));
  Serial.println(F("Pump: D9 | Trigger: SELECT (A6) | Limit: 5s"));

  updateDisplay(0);
}

void loop() {
  // 1. Button Detection
  Button currentBtn = readButton();

  if (currentBtn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // Trigger fixed duration run
    if (currentBtn == BTN_SELECT && !isPumpRunning) {
      isPumpRunning = true;
      pumpStartTime = millis();
      digitalWrite(PUMP_PIN, HIGH);
      Serial.println(F("[TRIGGER] Select pressed: Pump running for 5s"));
    }
    // Emergency Cutoff
    else if (currentBtn == BTN_BACK && isPumpRunning) {
      isPumpRunning = false;
      digitalWrite(PUMP_PIN, LOW);
      Serial.println(F("[ABORT] Back pressed: Pump stopped immediately"));
      updateDisplay(0);
    }
  }
  lastButton = currentBtn;

  // 2. Non-blocking Timer Handling
  if (isPumpRunning) {
    unsigned long elapsed = millis() - pumpStartTime;

    if (elapsed >= RUN_DURATION_MS) {
      // Time over -> auto stop
      isPumpRunning = false;
      digitalWrite(PUMP_PIN, LOW);
      Serial.println(F("[TIMER COMPLETE] 5s finished: Pump auto-stopped"));
      updateDisplay(0);
    } else {
      // Calculate remaining whole seconds
      int secondsRemaining = (int)((RUN_DURATION_MS - elapsed + 999) / 1000);
      updateDisplay(secondsRemaining);
    }
  }
}