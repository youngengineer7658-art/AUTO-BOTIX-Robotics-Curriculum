/*
==================================================
Day 41 | Practical-81: Control Pump Output
Board  : Young Engineer Board
Hardware:
  - Pump Driver Pin: D9
  - Display        : 0.96" I2C OLED (U8g2 - RAM Safe)
Baud Rate: 9600
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// 1. PIN DEFINITION
#define PUMP_PIN 9 // Dedicated Pump control output

// 2. RAM-SAFE OLED CONSTRUCTOR (Takes only 128 bytes RAM)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 3. TIMING VARIABLES (Non-blocking)
const unsigned long INTERVAL_MS = 3000; // 3 seconds ON, 3 seconds OFF
unsigned long lastToggleTime = 0;
bool pumpState = false;

//-------------------------------------------
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-81: PUMP");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 36, "OUTPUT PIN : D9");

    u8g2.drawStr(5, 54, "STATUS     :");
    if (pumpState) {
      u8g2.drawStr(75, 54, "RUNNING");
    } else {
      u8g2.drawStr(75, 54, "STOPPED");
    }
  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Start safely with pump OFF

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 41 | PRACTICAL-81: CONTROL PUMP    "));
  Serial.println(F("========================================"));
  Serial.println(F("Configured: Pump Pin = D9"));
  
  updateDisplay();
}

//-------------------------------------------
void loop() {
  if (millis() - lastToggleTime >= INTERVAL_MS) {
    lastToggleTime = millis();
    pumpState = !pumpState;

    digitalWrite(PUMP_PIN, pumpState ? HIGH : LOW);

    updateDisplay();

    Serial.print(F("Pump State: "));
    Serial.println(pumpState ? F("ON (Running)") : F("OFF (Idle)"));
  }
}