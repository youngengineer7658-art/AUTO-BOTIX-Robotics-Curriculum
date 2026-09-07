/*
==================================================
Day 41 | Practical-82: Run Pump Via Button
Board  : Young Engineer Board
Hardware:
  - Pump Driver Pin : D9
  - Button Pin      : A6 (Exact Calibrated Values)
  - Display         : 0.96" I2C OLED (U8g2 - RAM Safe)

Calibrated Values (A6):
  - Select (K1): 931-932 -> Toggle Pump ON / OFF
  - Up     (K2): 853-855
  - Down   (K3): 789-790
  - Back   (K4): 733-734 -> Emergency Stop (Pump OFF)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Ladder Ranges (With Safety Margins)
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

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool pumpActive = false;
Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void updateDisplay(const char* btnName) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-82: PUMP");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 34, "BUTTON :");
    u8g2.drawStr(65, 34, btnName);

    u8g2.drawStr(5, 52, "PUMP   :");
    if (pumpActive) {
      u8g2.drawStr(65, 52, "RUNNING");
    } else {
      u8g2.drawStr(65, 52, "STOPPED");
    }

    u8g2.drawStr(5, 63, "SEL: TOGGLE | BACK: OFF");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);
  digitalWrite(PUMP_PIN, LOW); // Start with pump OFF

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 41 | PRACTICAL-82: RUN PUMP BY BTN "));
  Serial.println(F("========================================"));
  Serial.println(F("Config: SELECT (931-932) Toggle | BACK (733-734) Stop"));

  updateDisplay("NONE");
}

void loop() {
  Button currentBtn = readButton();

  if (currentBtn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    if (currentBtn == BTN_SELECT) {
      pumpActive = !pumpActive;
      digitalWrite(PUMP_PIN, pumpActive ? HIGH : LOW);
      updateDisplay("SELECT");

      Serial.print(F("[BTN EVENT] SELECT Pressed -> Pump: "));
      Serial.println(pumpActive ? F("ON") : F("OFF"));
    } 
    else if (currentBtn == BTN_BACK) {
      pumpActive = false;
      digitalWrite(PUMP_PIN, LOW);
      updateDisplay("BACK (OFF)");

      Serial.println(F("[BTN EVENT] BACK Pressed -> Pump: FORCED OFF"));
    }
  }

  lastButton = currentBtn;
}