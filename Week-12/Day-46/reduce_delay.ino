/*
==================================================
Day 46 | Practical-92: Reduce Delays (Zero-Delay Scheduling)
Board  : Young Engineer Board
Pinout:
  - Ultrasonic Sensor : Trig D5, Echo A3
  - IR Sensor         : Pin A0
  - OLED Display      : 0.96" I2C (U8g2 - RAM Safe)

Objective:
  - Remove all blocking delay() calls
  - Independent task timers using millis()
  - Concurrent sensing and display refresh
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_PIN   A0

// RAM-Safe U8g2 Constructor (128-byte page buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Non-blocking Task Intervals
const unsigned long INTERVAL_SENSE   = 100; // Read sensors every 100ms
const unsigned long INTERVAL_DISPLAY = 200; // Update OLED every 200ms

unsigned long prevSenseTime   = 0;
unsigned long prevDisplayTime = 0;

int distanceCm = 0;
bool irHit     = false;

void readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 20ms safe timeout (prevents processor lockup)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 20000UL);
  if (duration == 0 || duration > 18000UL) {
    distanceCm = -1;
  } else {
    distanceCm = (int)((duration * 0.034) / 2);
  }
}

void readIR() {
  irHit = (analogRead(IR_PIN) < 500);
}

void drawDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "P-92: REDUCE DELAYS");
    u8g2.drawHLine(0, 15, 128);

    // Ultrasonic Reading
    u8g2.drawStr(5, 35, "US DIST :");
    if (distanceCm == -1) {
      u8g2.drawStr(65, 35, "-- cm");
    } else {
      char b[6];
      itoa(distanceCm, b, 10);
      u8g2.drawStr(65, 35, b);
      u8g2.drawStr(95, 35, "cm");
    }

    // IR Status
    u8g2.drawStr(5, 55, "IR (A0) :");
    u8g2.drawStr(65, 55, irHit ? "OBSTACLE" : "CLEAR");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 46 | PRACTICAL-92: REDUCE DELAYS   "));
  Serial.println(F("========================================"));
}

void loop() {
  unsigned long now = millis();

  // Task 1: Non-blocking Sensing (100ms)
  if (now - prevSenseTime >= INTERVAL_SENSE) {
    prevSenseTime = now;
    readUltrasonic();
    readIR();
  }

  // Task 2: Non-blocking Display & Telemetry (200ms)
  if (now - prevDisplayTime >= INTERVAL_DISPLAY) {
    prevDisplayTime = now;
    drawDisplay();

    Serial.print(F("Time: "));
    Serial.print(now);
    Serial.print(F("ms | Dist: "));
    if (distanceCm == -1) Serial.print(F("--"));
    else Serial.print(distanceCm);
    Serial.print(F(" cm | IR: "));
    Serial.println(irHit ? F("OBSTACLE") : F("CLEAR"));
  }
}