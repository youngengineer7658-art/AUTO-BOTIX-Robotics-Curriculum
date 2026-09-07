/*
==================================================
Day 38 | Practical-75: Detect Open Path Direction
Board  : Young Engineer Board / Auto-Botix
Sensors: Ultrasonic (Front: D5, A3) + 
         Left IR (A0) + Right IR (A1)
Display: OLED (U8g2)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// 1. PIN DEFINITIONS
#define TRIG_PIN     5
#define ECHO_PIN     A3
#define LEFT_IR_PIN  A0
#define RIGHT_IR_PIN A1

// 2. OLED OBJECT (128 bytes static RAM)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 3. THRESHOLDS
const int FRONT_OBSTACLE_DIST = 25; // in cm
const int IR_OBSTACLE_ADC     = 500; // ADC > 500 means blocked (adjust if active LOW)

// 4. VARIABLES
int frontDistance = 999;
bool leftBlocked = false;
bool rightBlocked = false;
bool frontBlocked = false;
const char* openPath = "FORWARD";

unsigned long lastSensorTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
int readFrontDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

//-------------------------------------------
void detectOpenPath() {
  // Read Sensors
  frontDistance = readFrontDistance();
  int leftVal = analogRead(LEFT_IR_PIN);
  int rightVal = analogRead(RIGHT_IR_PIN);

  // Evaluate Clear/Blocked status
  frontBlocked = (frontDistance > 0 && frontDistance <= FRONT_OBSTACLE_DIST);
  leftBlocked  = (leftVal > IR_OBSTACLE_ADC);
  rightBlocked = (rightVal > IR_OBSTACLE_ADC);

  // Determine Open Direction
  if (!frontBlocked) {
    openPath = "FORWARD";
  } 
  else if (!leftBlocked && rightBlocked) {
    openPath = "LEFT OPEN";
  } 
  else if (!rightBlocked && leftBlocked) {
    openPath = "RIGHT OPEN";
  } 
  else if (!leftBlocked && !rightBlocked) {
    openPath = "BOTH SIDES OPEN";
  } 
  else {
    openPath = "ALL BLOCKED (DEAD END)";
  }
}

//-------------------------------------------
void updateOLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 10, "PATH DETECTION");
    u8g2.drawHLine(0, 13, 128);

    // Front Distance
    u8g2.drawStr(5, 26, "FRONT:");
    if (frontDistance >= 400 || frontDistance <= 0 || frontDistance == 999) {
      u8g2.drawStr(55, 26, "--");
    } else {
      char buf[8];
      itoa(frontDistance, buf, 10);
      u8g2.drawStr(55, 26, buf);
      u8g2.drawStr(85, 26, "cm");
    }

    // Side Status
    u8g2.drawStr(5, 38, "L / R:");
    u8g2.drawStr(55, 38, leftBlocked ? "BLOCKED" : "CLEAR");
    u8g2.drawStr(98, 38, rightBlocked ? "| B" : "| C");

    // Detected Path
    u8g2.drawStr(5, 50, "PATH :");
    u8g2.drawStr(5, 62, openPath);
  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" PRACTICAL-75: DETECT OPEN PATH ACTIVE "));
  Serial.println(F("========================================"));

  updateOLED();
}

//-------------------------------------------
void loop() {
  // 1. SENSOR SCANNING (Every 100ms)
  if (millis() - lastSensorTime >= 100) {
    lastSensorTime = millis();
    detectOpenPath();
  }

  // 2. OLED TELEMETRY REFRESH (Every 200ms)
  if (millis() - lastDisplayTime >= 200) {
    lastDisplayTime = millis();
    updateOLED();
  }

  // 3. SERIAL MONITOR FEEDBACK (Every 800ms)
  if (millis() - lastSerialTime >= 800) {
    lastSerialTime = millis();
    Serial.print(F("F: "));
    if (frontDistance == 999) Serial.print(F("--"));
    else Serial.print(frontDistance);
    Serial.print(F(" cm | Left: "));
    Serial.print(leftBlocked ? F("BLOCKED") : F("CLEAR  "));
    Serial.print(F(" | Right: "));
    Serial.print(rightBlocked ? F("BLOCKED") : F("CLEAR  "));
    Serial.print(F(" | Open: "));
    Serial.println(openPath);
  }
}
