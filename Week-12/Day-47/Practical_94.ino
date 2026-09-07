/*
==================================================
Day 47 | Practical-94: Error Trapping & Debugging
Board  : Young Engineer Board
Pinout : Ultrasonic (D5, A3) | IR (A0) | OLED (I2C)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_PIN   A0

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

enum ErrorCode { ERR_NONE = 0, ERR_US_TIMEOUT = 1, ERR_IR_FAULT = 2 };
ErrorCode activeError = ERR_NONE;

int distanceCm = 0;
int irRaw = 0;

void pollSensorsWithValidation() {
  activeError = ERR_NONE;

  // Validate Ultrasonic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (duration == 0) {
    activeError = ERR_US_TIMEOUT;
    distanceCm = -1;
  } else {
    distanceCm = (int)((duration * 0.034) / 2);
  }

  // Validate IR
  irRaw = analogRead(IR_PIN);
  if (irRaw < 5 || irRaw > 1018) {
    activeError = ERR_IR_FAULT;
  }
}

void renderDebugUI() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "P-94: DEBUG MONITOR");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 32, "STATUS :");
    if (activeError == ERR_NONE) {
      u8g2.drawStr(65, 32, "ALL OK");
    } else {
      u8g2.drawStr(65, 32, "FAULT !");
    }

    u8g2.drawStr(5, 48, "CODE   :");
    switch (activeError) {
      case ERR_US_TIMEOUT: u8g2.drawStr(65, 48, "E:01 US_FAIL"); break;
      case ERR_IR_FAULT:   u8g2.drawStr(65, 48, "E:02 IR_FAIL"); break;
      default:             u8g2.drawStr(65, 48, "E:00 NONE");    break;
    }

    u8g2.drawStr(5, 62, "RAW US:");
    char b[5]; itoa(distanceCm, b, 10);
    u8g2.drawStr(65, 62, b);
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();
  Serial.println(F("=== P-94: ERROR MONITOR ACTIVE ==="));
}

void loop() {
  pollSensorsWithValidation();
  renderDebugUI();

  if (activeError != ERR_NONE) {
    Serial.print(F("[ALERT] Fault Detected -> Code: "));
    Serial.print((int)activeError);
    Serial.print(F(" | US: "));
    Serial.print(distanceCm);
    Serial.print(F(" | IR: "));
    Serial.println(irRaw);
  } else {
    Serial.print(F("[HEALTHY] US: "));
    Serial.print(distanceCm);
    Serial.print(F("cm | IR: "));
    Serial.println(irRaw);
  }

  delay(250);
}
