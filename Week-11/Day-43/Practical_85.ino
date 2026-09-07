/*
==================================================
Day 43 | Practical-85: Show Pump Status on OLED
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin : D9
  - Display         : 0.96" I2C OLED (U8g2 - RAM Safe)

Monitoring Features:
  - Visual status indicator (RUNNING / IDLE)
  - Live cycle runtime countdown timer
  - Total completed watering cycles counter
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN 9

// RAM-Safe OLED Constructor (128 bytes buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Timing Parameters
const unsigned long PUMP_RUN_MS  = 3000; // 3 seconds ON
const unsigned long PUMP_IDLE_MS = 5000; // 5 seconds OFF

unsigned long phaseStartTime = 0;
bool isPumpOn = false;
unsigned int cycleCount = 0;

void setPump(bool state) {
  isPumpOn = state;
  digitalWrite(PUMP_PIN, isPumpOn ? HIGH : LOW);
  phaseStartTime = millis();
}

void updateOLED(int secondsLeft) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-85: MONITOR");
    u8g2.drawHLine(0, 15, 128);

    // Pump Status
    u8g2.drawStr(5, 30, "PUMP   :");
    if (isPumpOn) {
      u8g2.drawStr(60, 30, "RUNNING");
    } else {
      u8g2.drawStr(60, 30, "IDLE");
    }

    // Live Phase Countdown
    u8g2.drawStr(5, 45, "TIMER  :");
    char timeBuf[5];
    itoa(secondsLeft, timeBuf, 10);
    u8g2.drawStr(60, 45, timeBuf);
    u8g2.drawStr(80, 45, "s left");

    // Total Cycles Completed
    u8g2.drawStr(5, 60, "CYCLES :");
    char cycBuf[6];
    itoa(cycleCount, cycBuf, 10);
    u8g2.drawStr(60, 60, cycBuf);
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  setPump(false); // Start with pump in IDLE state

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 43 | PRACTICAL-85: PUMP STATUS OLED"));
  Serial.println(F("========================================"));
  Serial.println(F("Pump: D9 | Display: I2C OLED (U8g2)"));

  updateOLED(PUMP_IDLE_MS / 1000);
}

void loop() {
  unsigned long elapsed = millis() - phaseStartTime;

  if (isPumpOn) {
    if (elapsed >= PUMP_RUN_MS) {
      // Watering complete -> switch to IDLE
      setPump(false);
      cycleCount++;
      Serial.print(F("[STATUS] Pump switched to IDLE | Cycles completed: "));
      Serial.println(cycleCount);
    } else {
      int secLeft = (int)((PUMP_RUN_MS - elapsed + 999) / 1000);
      updateOLED(secLeft);
    }
  } else {
    if (elapsed >= PUMP_IDLE_MS) {
      // Idle period over -> switch to RUNNING
      setPump(true);
      Serial.println(F("[STATUS] Pump switched to RUNNING"));
    } else {
      int secLeft = (int)((PUMP_IDLE_MS - elapsed + 999) / 1000);
      updateOLED(secLeft);
    }
  }
}
