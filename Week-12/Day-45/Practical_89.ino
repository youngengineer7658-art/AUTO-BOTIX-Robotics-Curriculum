/*
==================================================
Day 45 | Practical-89: Combine Sensors + Display
Board  : Young Engineer Board
Hardware Pinout:
  - Ultrasonic Trig     : Pin D5
  - Ultrasonic Echo     : Pin A3
  - IR Sensor (Analog)  : Pin A0
  - Soil Moisture Sensor: Pin A2
  - Display             : 0.96" I2C OLED (U8g2 - RAM Safe)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_PIN   A0
#define SOIL_PIN A2

// RAM-Safe U8g2 Constructor (128-byte page buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Calibration limits for soil sensor
const int SOIL_DRY_RAW = 850;
const int SOIL_WET_RAW = 350;

// IR Threshold (Analog detection)
const int IR_DETECT_THRESHOLD = 500;

int distanceCm  = 0;
int irRawVal    = 0;
bool irDetected = false;
int soilPercent = 0;

unsigned long lastSensorRead = 0;
const unsigned long readInterval = 300; // 300ms stable refresh

int readDistance() {
  // Line settle
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  // 10us Trigger Pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read Echo on A3 (40ms timeout)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 40000UL);

  if (duration == 0 || duration > 30000UL) {
    return -1; // Out of range ya timeout
  }

  int cm = (int)((duration * 0.034) / 2);
  if (cm < 2 || cm > 400) return -1;
  return cm;
}

void readIR() {
  irRawVal = analogRead(IR_PIN);
  irDetected = (irRawVal < IR_DETECT_THRESHOLD);
}

int readSoil() {
  int raw = analogRead(SOIL_PIN);
  int pct = map(raw, SOIL_DRY_RAW, SOIL_WET_RAW, 0, 100);
  return constrain(pct, 0, 100);
}

void updateDashboard() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-89: SENSORS");
    u8g2.drawHLine(0, 15, 128);

    // Ultrasonic Distance (Trig D5, Echo A3)
    u8g2.drawStr(5, 30, "DIST :");
    if (distanceCm == -1) {
      u8g2.drawStr(60, 30, "-- cm");
    } else {
      char dBuf[6];
      itoa(distanceCm, dBuf, 10);
      u8g2.drawStr(60, 30, dBuf);
      u8g2.drawStr(95, 30, "cm");
    }

    // IR Sensor on A0
    u8g2.drawStr(5, 45, "IR A0:");
    if (irDetected) {
      u8g2.drawStr(60, 45, "DETECTED");
    } else {
      u8g2.drawStr(60, 45, "CLEAR");
    }

    // Soil Moisture on A2
    u8g2.drawStr(5, 60, "SOIL :");
    char sBuf[6];
    itoa(soilPercent, sBuf, 10);
    u8g2.drawStr(60, 60, sBuf);
    u8g2.drawStr(95, 60, "%");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 45 | PRACTICAL-89: SENSORS DASHBOARD"));
  Serial.println(F("========================================"));
  Serial.println(F("Trig: D5 | Echo: A3 | IR: A0 | Soil: A2"));
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >= readInterval) {
    lastSensorRead = currentMillis;

    distanceCm  = readDistance();
    readIR();
    soilPercent = readSoil();

    updateDashboard();

    // Serial Monitor Telemetry
    Serial.print(F("Dist: "));
    if (distanceCm == -1) Serial.print(F("Out/Err"));
    else { Serial.print(distanceCm); Serial.print(F(" cm")); }
    Serial.print(F(" | IR A0: "));
    Serial.print(irDetected ? F("DETECTED") : F("CLEAR"));
    Serial.print(F(" | Soil A2: "));
    Serial.print(soilPercent);
    Serial.println(F("%"));
  }
}
