/*
==================================================
Day 46 | Practical-91: Code Cleanup & Memory Opt
Board  : Young Engineer Board
Pinout : OLED I2C (A4-SDA, A5-SCL)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// RAM-Safe U8g2 128-byte page buffer
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

struct SystemMetrics {
  unsigned long loopCount;
  uint16_t freeRamBytes;
};

SystemMetrics sysMetrics;
unsigned long lastTick = 0;

int getFreeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void renderCleanUI() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "P-91: CODE CLEANUP");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 35, "FREE RAM :");
    char ramBuf[7];
    itoa(sysMetrics.freeRamBytes, ramBuf, 10);
    u8g2.drawStr(75, 35, ramBuf);
    u8g2.drawStr(110, 35, "B");

    u8g2.drawStr(5, 55, "STATUS   : OPTIMIZED");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  Serial.println(F("[SYSTEM] Clean Architecture Initialized"));
}

void loop() {
  unsigned long now = millis();
  sysMetrics.loopCount++;

  if (now - lastTick >= 500) {
    lastTick = now;
    sysMetrics.freeRamBytes = getFreeRam();

    renderCleanUI();

    Serial.print(F("[P-91] Free RAM: "));
    Serial.print(sysMetrics.freeRamBytes);
    Serial.println(F(" Bytes"));
  }
}