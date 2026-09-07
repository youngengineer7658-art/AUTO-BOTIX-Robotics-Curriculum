/*
==================================================
Day 40 | Practical-80: Optimize Movement
Board  : Young Engineer Board / Auto-Botix (Schematic Verified)
Hardware:
  - Motors: L293D (M11: 2, M12: 4, EN1: 6, M21: 7, M22: 8, EN2: 9)
  - Ultrasonic: Trig (Pin 5), Echo (Pin A3)
  - OLED Display: I2C via U8g2 (128 bytes RAM safe)
Logic:
  - Non-blocking smooth PWM acceleration / deceleration (Anti-slip / Anti-jerk)
  - Fast obstacle avoidance escape: Stop -> Back (700ms) -> Turn (500ms)
  - Telemetry streaming to OLED and Serial Monitor
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// 1. PIN DEFINITIONS (Schematic Matched)
#define TRIG_PIN 5
#define ECHO_PIN A3

// L293D Motor Pins
#define M11      2
#define M12      4
#define M_EN     6   // Left Motor PWM
#define M21      7
#define M22      8
#define MOUT1    9   // Right Motor PWM

// 2. RAM-SAFE OLED CONSTRUCTOR
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 3. PARAMETERS FOR MOVEMENT OPTIMIZATION
const int MAX_CRUISE_SPEED = 220; // Smooth cruise speed
const int MIN_START_SPEED  = 120; // Overcomes initial static friction
const int OBSTACLE_DIST    = 20;  // Threshold in cm
const int RAMP_STEP        = 8;   // PWM increment per tick

int currentSpeed = 0;
int targetSpeed  = 0;
int distance     = 999;
const char* actionState = "IDLE";

unsigned long lastRampTime   = 0;
unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
// MOTOR CONTROL
void setMotorOutputs(byte m11, byte m12, byte m21, byte m22) {
  digitalWrite(M11, m11); digitalWrite(M12, m12);
  digitalWrite(M21, m21); digitalWrite(M22, m22);
}

void applySpeed(int spd) {
  analogWrite(M_EN, spd);
  analogWrite(MOUT1, spd);
}

// Smooth Non-blocking Speed Ramping
void processSpeedRamp() {
  if (millis() - lastRampTime >= 15) { // Update every 15ms
    lastRampTime = millis();
    
    if (currentSpeed < targetSpeed) {
      currentSpeed += RAMP_STEP;
      if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
    } else if (currentSpeed > targetSpeed) {
      currentSpeed -= RAMP_STEP;
      if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
    }
    applySpeed(currentSpeed);
  }
}

void hardStop() {
  setMotorOutputs(LOW, LOW, LOW, LOW);
  currentSpeed = 0;
  targetSpeed  = 0;
  applySpeed(0);
}

//-------------------------------------------
// ULTRASONIC READING
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000); // 20ms timeout
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

//-------------------------------------------
// OPTIMIZED ESCAPE MANEUVER
void executeAvoidanceManeuver() {
  // 1. Soft Brake
  targetSpeed = 0;
  while (currentSpeed > 0) {
    processSpeedRamp();
  }
  hardStop();
  actionState = "BRAKE";
  updateDisplay();
  delay(100);

  // 2. Controlled Reverse
  setMotorOutputs(LOW, HIGH, LOW, HIGH);
  targetSpeed = 200;
  currentSpeed = MIN_START_SPEED;
  applySpeed(currentSpeed);
  actionState = "REVERSE";
  updateDisplay();
  delay(700); // 700ms safe back-up

  // 3. Quick Pivot Turn
  setMotorOutputs(HIGH, LOW, LOW, HIGH); // Turn Right
  actionState = "PIVOT TURN";
  updateDisplay();
  delay(500);

  hardStop();
}

//-------------------------------------------
// OLED DASHBOARD
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "OPTIMIZE MOVEMENT");
    u8g2.drawHLine(0, 15, 128);

    // Distance
    u8g2.drawStr(5, 32, "DIST  :");
    if (distance >= 999 || distance <= 0) {
      u8g2.drawStr(60, 32, "-- cm");
    } else {
      char buf[6];
      itoa(distance, buf, 10);
      u8g2.drawStr(60, 32, buf);
      u8g2.drawStr(90, 32, "cm");
    }

    // PWM Speed
    u8g2.drawStr(5, 48, "SPEED :");
    char spdBuf[6];
    itoa(currentSpeed, spdBuf, 10);
    u8g2.drawStr(60, 48, spdBuf);
    u8g2.drawStr(90, 48, "PWM");

    // Action State
    u8g2.drawStr(5, 62, "ACT   :");
    u8g2.drawStr(60, 62, actionState);

  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M_EN, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MOUT1, OUTPUT);

  hardStop();
  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 40 | PRACTICAL-80: OPTIMIZE MOVE   "));
  Serial.println(F("========================================"));
  
  actionState = "READY";
  updateDisplay();
}

//-------------------------------------------
void loop() {
  // 1. NON-BLOCKING SPEED RAMPING (Runs constantly)
  processSpeedRamp();

  // 2. ULTRASONIC SENSOR SAMPLING (Every 60ms)
  if (millis() - lastSensorTime >= 60) {
    lastSensorTime = millis();
    distance = readDistance();

    if (distance > 0 && distance <= OBSTACLE_DIST) {
      executeAvoidanceManeuver();
    } else {
      // Clear path: set forward and ramp up
      setMotorOutputs(HIGH, LOW, HIGH, LOW);
      targetSpeed = MAX_CRUISE_SPEED;
      actionState = "FORWARD (RAMP)";
    }
    updateDisplay();
  }

  // 3. SERIAL LOGGING (Every 500ms)
  if (millis() - lastSerialTime >= 500) {
    lastSerialTime = millis();
    Serial.print(F("Dist: "));
    if (distance == 999) Serial.print(F("--"));
    else Serial.print(distance);
    Serial.print(F(" cm | PWM: "));
    Serial.print(currentSpeed);
    Serial.print(F(" | Action: "));
    Serial.println(actionState);
  }
}