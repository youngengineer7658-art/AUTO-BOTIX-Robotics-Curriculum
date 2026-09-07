/*
==================================================
Day 43 | Practical-86: Add Manual Pump Override Button
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Ladder Pin : A6 (Calibrated Values)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Calibrated Button Values (A6):
  - Select (925-938) : Toggle Manual Override (Force ON / Force OFF)
  - Back   (728-740) : Reset to AUTO Cycle Mode

Logic:
  - Default AUTO cyclic timer (4s RUN / 6s WAIT)
  - Manual override instantly takes control without freezing timer logic
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

// Timing Parameters for Auto Cycle
const unsigned long AUTO_RUN_MS  = 4000; // 4s ON
const unsigned long AUTO_WAIT_MS = 6000; // 6s OFF

unsigned long stateStartTime = 0;
bool isPumpRunning  = false;
bool manualOverride = false; // False = AUTO, True = MANUAL OVERRIDE

Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setPump(bool state) {
  isPumpRunning = state;
  digitalWrite(PUMP_PIN, isPumpRunning ? HIGH : LOW);
  stateStartTime = millis();
}

void updateOLED(int secondsLeft) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-86: OVERRIDE");
    u8g2.drawHLine(0, 15, 128);

    // Mode Display
    u8g2.drawStr(5, 30, "MODE   :");
    if (manualOverride) {
      u8g2.drawStr(60, 30, "MANUAL");
    } else {
      u8g2.drawStr(60, 30, "AUTO CYCLE");
    }

    // Pump Status
    u8g2.drawStr(5, 45, "PUMP   :");
    if (isPumpRunning) {
      u8g2.drawStr(60, 45, "ON");
    } else {
      u8g2.drawStr(60, 45, "OFF");
    }

    // Detail Line
    if (manualOverride) {
      u8g2.drawStr(5, 60, "SEL:TOGGLE | BACK:AUTO");
    } else {
      u8g2.drawStr(5, 60, "NEXT IN:");
      char secBuf[5];
      itoa(secondsLeft, secBuf, 10);
      u8g2.drawStr(60, 60, secBuf);
      u8g2.drawStr(80, 60, "s");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);

  u8g2.begin();

  // Start with pump OFF in AUTO mode
  setPump(false);

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 43 | PRACTICAL-86: MANUAL OVERRIDE "));
  Serial.println(F("========================================"));
  Serial.println(F("Select: Toggle Override | Back: Return to AUTO"));

  updateOLED(AUTO_WAIT_MS / 1000);
}

void loop() {
  // 1. Button Input Handling
  Button currentBtn = readButton();

  if (currentBtn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // Select: Enter manual mode and toggle pump
    if (currentBtn == BTN_SELECT) {
      manualOverride = true;
      setPump(!isPumpRunning);
      updateOLED(0);

      Serial.print(F("[MANUAL OVERRIDE] Select Pressed -> Pump: "));
      Serial.println(isPumpRunning ? F("ON") : F("OFF"));
    }
    // Back: Return back to AUTO cycle mode
    else if (currentBtn == BTN_BACK) {
      manualOverride = false;
      setPump(false); // Reset to waiting state in AUTO
      updateOLED(AUTO_WAIT_MS / 1000);

      Serial.println(F("[MODE SWITCH] Back Pressed -> Returned to AUTO Cycle"));
    }
  }
  lastButton = currentBtn;

  // 2. Automatic Cycle Execution (Only active when NOT in manual override)
  if (!manualOverride) {
    unsigned long elapsed = millis() - stateStartTime;

    if (isPumpRunning) {
      if (elapsed >= AUTO_RUN_MS) {
        setPump(false);
        Serial.println(F("[AUTO] Watering finished -> Switched to WAIT"));
      } else {
        int secLeft = (int)((AUTO_RUN_MS - elapsed + 999) / 1000);
        updateOLED(secLeft);
      }
    } else {
      if (elapsed >= AUTO_WAIT_MS) {
        setPump(true);
        Serial.println(F("[AUTO] Wait complete -> Switched to WATERING"));
      } else {
        int secLeft = (int)((AUTO_WAIT_MS - elapsed + 999) / 1000);
        updateOLED(secLeft);
      }
    }
  }
}
