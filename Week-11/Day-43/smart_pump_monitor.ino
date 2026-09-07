/*
==================================================
Day 43 | Project-43: Smart Pump Monitor
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Ladder Pin : A6 (Calibrated Values)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Calibrated Button Values (A6):
  - Select (925-938) : Toggle Mode (AUTO <-> MANUAL)
  - Up     (848-860) : In MANUAL: Pump ON  | In AUTO: Increase Interval (+5s)
  - Down   (783-795) : In MANUAL: Pump OFF | In AUTO: Decrease Interval (-5s)
  - Back   (728-740) : Emergency Cutoff / Pause System

Telemetry & Health Dashboard on OLED:
  - Mode: AUTO / MANUAL / PAUSED
  - Pump State: ON / OFF
  - Total Cumulative Runtime (seconds)
  - Total Completed Cycles
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Ladder Ranges
#define BTN_SEL_MIN  925
#define BTN_SEL_MAX  938
#define BTN_UP_MIN   848
#define BTN_UP_MAX   860
#define BTN_DOWN_MIN 783
#define BTN_DOWN_MAX 795
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

enum Button { BTN_NONE, BTN_SELECT, BTN_UP, BTN_DOWN, BTN_BACK };

Button readButton() {
  int val = analogRead(BUTTONS_PIN);
  if (val >= BTN_SEL_MIN  && val <= BTN_SEL_MAX)  return BTN_SELECT;
  if (val >= BTN_UP_MIN   && val <= BTN_UP_MAX)   return BTN_UP;
  if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX) return BTN_DOWN;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

// RAM-Safe OLED Constructor (128 bytes buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// System Modes
enum SystemMode { MODE_AUTO, MODE_MANUAL, MODE_PAUSED };
SystemMode currentMode = MODE_AUTO;

// Configurable Parameters
const unsigned long RUN_BURST_MS = 3000;       // 3 seconds watering
unsigned long waitIntervalMs     = 15000;      // 15 seconds default wait

// Runtime Tracking & Health Metrics
unsigned long phaseStartTime   = 0;
unsigned long pumpTurnedOnAt   = 0;
unsigned long totalRunSeconds  = 0;
unsigned int totalCycles       = 0;
bool isPumpRunning             = false;

Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setPump(bool state) {
  if (state && !isPumpRunning) {
    pumpTurnedOnAt = millis();
  } else if (!state && isPumpRunning) {
    totalRunSeconds += (millis() - pumpTurnedOnAt) / 1000;
  }
  isPumpRunning = state;
  digitalWrite(PUMP_PIN, isPumpRunning ? HIGH : LOW);
  phaseStartTime = millis();
}

void updateDashboard(int secondsLeft) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "SMART PUMP MONITOR");
    u8g2.drawHLine(0, 15, 128);

    // Line 1: Mode & Pump Status
    u8g2.drawStr(5, 29, "MODE:");
    if (currentMode == MODE_AUTO) {
      u8g2.drawStr(45, 29, "AUTO");
    } else if (currentMode == MODE_MANUAL) {
      u8g2.drawStr(45, 29, "MANUAL");
    } else {
      u8g2.drawStr(45, 29, "PAUSED");
    }

    u8g2.drawStr(90, 29, isPumpRunning ? "[ON]" : "[OFF]");

    // Line 2: Next Action Countdown / Control Hint
    if (currentMode == MODE_AUTO) {
      u8g2.drawStr(5, 43, isPumpRunning ? "RUNNING:" : "WAITING:");
      char secBuf[5];
      itoa(secondsLeft, secBuf, 10);
      u8g2.drawStr(65, 43, secBuf);
      u8g2.drawStr(85, 43, "s");
    } else if (currentMode == MODE_MANUAL) {
      u8g2.drawStr(5, 43, "UP:ON | DOWN:OFF");
    } else {
      u8g2.drawStr(5, 43, "BACK:RESUME AUTO");
    }

    // Line 3: System Analytics (Total Run Time & Cycles)
    u8g2.drawStr(5, 58, "TIME:");
    char runBuf[6];
    itoa(totalRunSeconds, runBuf, 10);
    u8g2.drawStr(40, 58, runBuf);
    u8g2.drawStr(60, 58, "s");

    u8g2.drawStr(78, 58, "CYC:");
    char cycBuf[5];
    itoa(totalCycles, cycBuf, 10);
    u8g2.drawStr(108, 58, cycBuf);
  } while (u8g2.nextPage());
}

void handleButtons() {
  Button btn = readButton();

  if (btn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // Select: Toggle AUTO <-> MANUAL
    if (btn == BTN_SELECT) {
      if (currentMode == MODE_AUTO) {
        currentMode = MODE_MANUAL;
        setPump(false);
      } else {
        currentMode = MODE_AUTO;
        setPump(false);
      }
      Serial.print(F("[MODE] Switched to: "));
      Serial.println(currentMode == MODE_AUTO ? F("AUTO") : F("MANUAL"));
    }
    // Up Button
    else if (btn == BTN_UP) {
      if (currentMode == MODE_MANUAL) {
        setPump(true);
        Serial.println(F("[MANUAL] Force ON"));
      } else if (currentMode == MODE_AUTO) {
        if (waitIntervalMs < 60000) waitIntervalMs += 5000;
        Serial.print(F("[CONFIG] Interval increased: "));
        Serial.print(waitIntervalMs / 1000);
        Serial.println(F("s"));
      }
    }
    // Down Button
    else if (btn == BTN_DOWN) {
      if (currentMode == MODE_MANUAL) {
        setPump(false);
        Serial.println(F("[MANUAL] Force OFF"));
      } else if (currentMode == MODE_AUTO) {
        if (waitIntervalMs > 5000) waitIntervalMs -= 5000;
        Serial.print(F("[CONFIG] Interval decreased: "));
        Serial.print(waitIntervalMs / 1000);
        Serial.println(F("s"));
      }
    }
    // Back Button: Pause / Abort
    else if (btn == BTN_BACK) {
      if (currentMode == MODE_PAUSED) {
        currentMode = MODE_AUTO;
        setPump(false);
        Serial.println(F("[OVERRIDE] Resumed to AUTO"));
      } else {
        currentMode = MODE_PAUSED;
        setPump(false);
        Serial.println(F("[EMERGENCY] System PAUSED"));
      }
    }
  }
  lastButton = btn;
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);

  u8g2.begin();
  setPump(false);

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 43 | PROJECT-43: SMART PUMP MONITOR"));
  Serial.println(F("========================================"));
  Serial.println(F("SEL: Mode | UP/DN: Adjust/Control | BACK: Pause"));

  updateDashboard(waitIntervalMs / 1000);
}

void loop() {
  handleButtons();

  if (currentMode == MODE_AUTO) {
    unsigned long elapsed = millis() - phaseStartTime;

    if (isPumpRunning) {
      if (elapsed >= RUN_BURST_MS) {
        setPump(false);
        totalCycles++;
        Serial.print(F("[AUTO] Run Done. Completed Cycles: "));
        Serial.println(totalCycles);
      } else {
        int secLeft = (int)((RUN_BURST_MS - elapsed + 999) / 1000);
        updateDashboard(secLeft);
      }
    } else {
      if (elapsed >= waitIntervalMs) {
        setPump(true);
        Serial.println(F("[AUTO] Wait Complete -> Starting Pump"));
      } else {
        int secLeft = (int)((waitIntervalMs - elapsed + 999) / 1000);
        updateDashboard(secLeft);
      }
    }
  } else {
    // If in MANUAL or PAUSED, keep live duration tracking updated
    if (isPumpRunning) {
      unsigned long liveSec = totalRunSeconds + ((millis() - pumpTurnedOnAt) / 1000);
      updateDashboard(0);
    } else {
      updateDashboard(0);
    }
  }
}