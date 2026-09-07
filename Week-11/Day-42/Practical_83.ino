/*
==================================================
Day 42 | Practical-84: Create Automatic Pump Timer
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Pin        : A6 (Back Button for Override/Stop)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Timer Logic (Non-blocking):
  - IDLE / WAIT Time : 10 Seconds (Interval between watering)
  - WATERING / RUN   : 3 Seconds (Active pump duration)
  - Back Button (728-740): Emergency Pause / Resume timer
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Back Button (Emergency Stop / Pause)
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Timing Intervals
const unsigned long PUMP_RUN_MS  = 3000;  // 3 seconds pump ON
const unsigned long PUMP_WAIT_MS = 10000; // 10 seconds pump OFF (Wait period)

unsigned long stateStartTime = 0;
bool isPumpRunning = false;
bool timerPaused   = false;

bool lastBackState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

bool isBackPressed() {
  int val = analogRead(BUTTONS_PIN);
  return (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX);
}

void setPump(bool state) {
  isPumpRunning = state;
  digitalWrite(PUMP_PIN, isPumpRunning ? HIGH : LOW);
  stateStartTime = millis();
}

void updateDisplay(int secondsRemaining) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-84: AUTO");
    u8g2.drawHLine(0, 15, 128);

    if (timerPaused) {
      u8g2.drawStr(5, 36, "TIMER   : PAUSED");
      u8g2.drawStr(5, 52, "PUMP    : OFF");
      u8g2.drawStr(5, 63, "BACK: RESUME TIMER");
    } else {
      u8g2.drawStr(5, 34, "STATE   :");
      if (isPumpRunning) {
        u8g2.drawStr(65, 34, "WATERING");
      } else {
        u8g2.drawStr(65, 34, "WAITING");
      }

      u8g2.drawStr(5, 50, "PUMP    :");
      u8g2.drawStr(65, 50, isPumpRunning ? "ON" : "OFF");

      u8g2.drawStr(5, 62, "NEXT IN :");
      char secBuf[5];
      itoa(secondsRemaining, secBuf, 10);
      u8g2.drawStr(65, 62, secBuf);
      u8g2.drawStr(85, 62, "s");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);

  u8g2.begin();

  // Start with pump OFF in WAIT state
  setPump(false);

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 42 | PRACTICAL-84: AUTO PUMP TIMER "));
  Serial.println(F("========================================"));
  Serial.println(F("Cycle: 3s ON / 10s OFF | Override: Back (A6)"));

  updateDisplay(10);
}

void loop() {
  // 1. Check Back Button (Pause / Resume toggle)
  bool backPressed = isBackPressed();
  if (backPressed && !lastBackState && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    timerPaused = !timerPaused;

    if (timerPaused) {
      setPump(false);
      Serial.println(F("[OVERRIDE] Timer Paused! Pump Stopped."));
    } else {
      stateStartTime = millis(); // Reset current timer phase
      Serial.println(F("[OVERRIDE] Timer Resumed."));
    }
    updateDisplay(0);
  }
  lastBackState = backPressed;

  // 2. Automatic Timer Logic (Runs when not paused)
  if (!timerPaused) {
    unsigned long currentElapsed = millis() - stateStartTime;

    if (isPumpRunning) {
      // Pump is currently running -> check if 3 seconds are up
      if (currentElapsed >= PUMP_RUN_MS) {
        setPump(false); // Switch to WAIT state
        Serial.println(F("[TIMER] 3s Watering Complete -> Switched to WAIT"));
      } else {
        int secLeft = (int)((PUMP_RUN_MS - currentElapsed + 999) / 1000);
        updateDisplay(secLeft);
      }
    } else {
      // Pump is currently waiting -> check if 10 seconds are up
      if (currentElapsed >= PUMP_WAIT_MS) {
        setPump(true); // Switch to RUN state
        Serial.println(F("[TIMER] 10s Wait Finished -> Switched to WATERING"));
      } else {
        int secLeft = (int)((PUMP_WAIT_MS - currentElapsed + 999) / 1000);
        updateDisplay(secLeft);
      }
    }
  }
}
