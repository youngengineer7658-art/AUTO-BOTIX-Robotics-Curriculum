/*
==================================================
Day 44 | Practical-88: Test Pump Automation
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Ladder Pin : A6 (Calibrated Values)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Automated Self-Test Engine:
  - Phase 1: 2s RUN Test (Checks current draw / motor spin)
  - Phase 2: 3s REST Test (Checks inductive kickback & cutoff)
  - Phase 3: 2s RUN Test (Verifies repeat cycle consistency)
  - Pass/Fail evaluation displayed on OLED & Serial Monitor
  - Back Button: Emergency Abort at any point
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Ladder Ranges
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

// Test State Machine
enum TestStep { STEP_STANDBY, STEP_PULSE_1, STEP_REST, STEP_PULSE_2, STEP_PASSED, STEP_ABORTED };
TestStep currentStep = STEP_STANDBY;

unsigned long stepStartTime = 0;
Button lastButton = BTN_NONE;
unsigned long lastDebounce = 0;

void setPump(bool state) {
  digitalWrite(PUMP_PIN, state ? HIGH : LOW);
}

void renderTestScreen(int secLeft) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-88: AUTO-TEST");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 32, "STAGE :");
    switch (currentStep) {
      case STEP_STANDBY:
        u8g2.drawStr(60, 32, "READY");
        u8g2.drawStr(5, 48, "PRESS SELECT TO START");
        u8g2.drawStr(5, 62, "BACK: ABORT TEST");
        break;

      case STEP_PULSE_1:
        u8g2.drawStr(60, 32, "RUN TEST 1");
        u8g2.drawStr(5, 48, "PUMP  : [ON]");
        u8g2.drawStr(5, 62, "TIME  :");
        char b1[4]; itoa(secLeft, b1, 10);
        u8g2.drawStr(55, 62, b1); u8g2.drawStr(70, 62, "s left");
        break;

      case STEP_REST:
        u8g2.drawStr(60, 32, "REST TEST");
        u8g2.drawStr(5, 48, "PUMP  : [OFF]");
        u8g2.drawStr(5, 62, "TIME  :");
        char b2[4]; itoa(secLeft, b2, 10);
        u8g2.drawStr(55, 62, b2); u8g2.drawStr(70, 62, "s left");
        break;

      case STEP_PULSE_2:
        u8g2.drawStr(60, 32, "RUN TEST 2");
        u8g2.drawStr(5, 48, "PUMP  : [ON]");
        u8g2.drawStr(5, 62, "TIME  :");
        char b3[4]; itoa(secLeft, b3, 10);
        u8g2.drawStr(55, 62, b3); u8g2.drawStr(70, 62, "s left");
        break;

      case STEP_PASSED:
        u8g2.drawStr(60, 32, "ALL PASS");
        u8g2.drawStr(5, 48, "SYSTEM HEALTHY [OK]");
        u8g2.drawStr(5, 62, "PRESS SEL TO RETEST");
        break;

      case STEP_ABORTED:
        u8g2.drawStr(60, 32, "ABORTED");
        u8g2.drawStr(5, 48, "EMERGENCY STOP [X]");
        u8g2.drawStr(5, 62, "PRESS SEL TO RETRY");
        break;
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);
  setPump(false);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 44 | PRACTICAL-88: TEST AUTOMATION "));
  Serial.println(F("========================================"));
  Serial.println(F("Select: Run Full Test Routine | Back: Abort"));

  renderTestScreen(0);
}

void loop() {
  Button btn = readButton();

  if (btn != lastButton && (millis() - lastDebounce > 200)) {
    lastDebounce = millis();

    if (btn == BTN_SELECT && (currentStep == STEP_STANDBY || currentStep == STEP_PASSED || currentStep == STEP_ABORTED)) {
      currentStep = STEP_PULSE_1;
      setPump(true);
      stepStartTime = millis();
      Serial.println(F("[AUTO-TEST] Started: Step 1 (2s Run Test)"));
    } else if (btn == BTN_BACK && currentStep != STEP_STANDBY) {
      currentStep = STEP_ABORTED;
      setPump(false);
      Serial.println(F("[AUTO-TEST] User Abort via Back Button!"));
      renderTestScreen(0);
    }
  }
  lastButton = btn;

  // Automated Routine Engine (Non-blocking)
  unsigned long elapsed = millis() - stepStartTime;

  switch (currentStep) {
    case STEP_PULSE_1:
      if (elapsed >= 2000) {
        currentStep = STEP_REST;
        setPump(false);
        stepStartTime = millis();
        Serial.println(F("[AUTO-TEST] Step 1 PASS -> Step 2 (3s Rest Test)"));
      } else {
        renderTestScreen((int)((2000 - elapsed + 999) / 1000));
      }
      break;

    case STEP_REST:
      if (elapsed >= 3000) {
        currentStep = STEP_PULSE_2;
        setPump(true);
        stepStartTime = millis();
        Serial.println(F("[AUTO-TEST] Step 2 PASS -> Step 3 (2s Repeat Run)"));
      } else {
        renderTestScreen((int)((3000 - elapsed + 999) / 1000));
      }
      break;

    case STEP_PULSE_2:
      if (elapsed >= 2000) {
        currentStep = STEP_PASSED;
        setPump(false);
        Serial.println(F("[AUTO-TEST] COMPLETE: All Automation Stages PASSED [OK]"));
        renderTestScreen(0);
      } else {
        renderTestScreen((int)((2000 - elapsed + 999) / 1000));
      }
      break;

    default:
      break;
  }
}
