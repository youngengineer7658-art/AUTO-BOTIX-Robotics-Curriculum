/*
==================================================
Day 40 | Practical-79: Sensor Calibration
Board  : Young Engineer Board / Auto-Botix (Schematic Verified)
Sensors:
  - Ultrasonic: Trig -> D5, Echo -> A3
  - Analog IR Left:  Pin A0
  - Analog IR Right: Pin A1
Display: 0.96" I2C OLED (U8g2 - RAM Safe)
Baud   : 9600
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// 1. PIN DEFINITIONS (Schematic Matched)
#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR1_PIN  A0 // Left Line/Proximity Sensor
#define IR2_PIN  A1 // Right Line/Proximity Sensor

// 2. RAM-SAFE OLED CONSTRUCTOR (Takes only 128 bytes RAM)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 3. CALIBRATION CONSTANTS
const float ULTRASONIC_OFFSET = 0.0; // cm adjustment
const int IR_THRESHOLD        = 500; // Above 500 = Black/Obstacle

// Telemetry & Filtering Variables
int currentDist = 0;
int ir1Val = 0;
int ir2Val = 0;
unsigned long lastSampleTime = 0;

//-------------------------------------------
int readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000); // 25ms timeout
  if (duration == 0) return 999;
  int cm = (duration * 0.0343) / 2;
  return cm + ULTRASONIC_OFFSET;
}

//-------------------------------------------
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "SENSOR CALIBRATION");
    u8g2.drawHLine(0, 15, 128);

    // Ultrasonic Row
    u8g2.drawStr(5, 30, "US DIST :");
    if (currentDist >= 999 || currentDist <= 0) {
      u8g2.drawStr(70, 30, "-- cm");
    } else {
      char buf[6];
      itoa(currentDist, buf, 10);
      u8g2.drawStr(70, 30, buf);
      u8g2.drawStr(95, 30, "cm");
    }

    // IR 1 Row
    u8g2.drawStr(5, 46, "IR1 (A0):");
    char buf1[6];
    itoa(ir1Val, buf1, 10);
    u8g2.drawStr(70, 46, buf1);
    if (ir1Val > IR_THRESHOLD) u8g2.drawStr(100, 46, "[!]");

    // IR 2 Row
    u8g2.drawStr(5, 61, "IR2 (A1):");
    char buf2[6];
    itoa(ir2Val, buf2, 10);
    u8g2.drawStr(70, 61, buf2);
    if (ir2Val > IR_THRESHOLD) u8g2.drawStr(100, 61, "[!]");

  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F("  DAY 40 | PRACTICAL-79: CALIBRATION    "));
  Serial.println(F("========================================"));
  Serial.println(F("Ultrasonic (D5, A3) | IR1 (A0) | IR2 (A1)"));
}

//-------------------------------------------
void loop() {
  // Sample every 150ms to keep display and serial responsive
  if (millis() - lastSampleTime >= 150) {
    lastSampleTime = millis();

    currentDist = readUltrasonic();
    ir1Val = analogRead(IR1_PIN);
    ir2Val = analogRead(IR2_PIN);

    bool obs1 = (ir1Val > IR_THRESHOLD);
    bool obs2 = (ir2Val > IR_THRESHOLD);

    // Live Serial Logging for Plotter / Debugging
    Serial.print(F("US_cm:"));
    if (currentDist == 999) Serial.print(F("-1"));
    else Serial.print(currentDist);

    Serial.print(F("\tIR1_A0:"));
    Serial.print(ir1Val);
    if (obs1) Serial.print(F("(*)"));

    Serial.print(F("\tIR2_A1:"));
    Serial.print(ir2Val);
    if (obs2) Serial.print(F("(*)"));
    Serial.println();

    updateDisplay();
  }
}