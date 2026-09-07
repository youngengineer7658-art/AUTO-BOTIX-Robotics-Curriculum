/*
==================================================
Day 39 | Practical-77: Manual & Working Auto Mode
Hardware: IR Receiver (D3) + Ultrasonic (D5, A3)
          L293D Motors (M11:2, M12:4, EN1:6, M21:7, M22:8, EN2:9)
          OLED Display (U8g2)
Telemetry: Detailed Serial Monitor Feedback (9600 Baud)
==================================================
*/

#define DECODE_NEC
#include <Wire.h>
#include <U8g2lib.h>
#include <IRremote.hpp>

// 1. PIN DEFINITIONS (From Schematic)
#define TSOP_PIN  3
#define TRIG_PIN  5
#define ECHO_PIN  A3

#define M11       2
#define M12       4
#define M_EN      6
#define M21       7
#define M22       8
#define MOUT1     9

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define IR_POWER  0x09
#define IR_OK     0x05

const int MOTOR_SPEED   = 210;
const int OBSTACLE_DIST = 25; // cm

bool autoMode = false;
const char* lastCommand = "STOP";
int distance = 999;

unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
void setMotorSpeed(int speed1, int speed2) {
  analogWrite(M_EN, speed1);
  analogWrite(MOUT1, speed2);
}

void stopMotors() {
  digitalWrite(M11, LOW); digitalWrite(M12, LOW);
  digitalWrite(M21, LOW); digitalWrite(M22, LOW);
  setMotorSpeed(0, 0);
}

void moveForward() {
  digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
  digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void turnLeft() {
  digitalWrite(M11, LOW);  digitalWrite(M12, HIGH);
  digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void turnRight() {
  digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);  digitalWrite(M22, HIGH);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

//-------------------------------------------
int readDistance() {
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
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 12, "PRACTICAL-77");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(10, 34, "MODE :");
    u8g2.drawStr(60, 34, autoMode ? "AUTO" : "MANUAL");

    u8g2.drawStr(10, 52, "ACT  :");
    u8g2.drawStr(60, 52, lastCommand);
  } while (u8g2.nextPage());
}

//-------------------------------------------
void handleIRCommand(byte code) {
  Serial.print(F("[IR RECV] Code: 0x"));
  if (code < 0x10) Serial.print(F("0"));
  Serial.print(code, HEX);

  if (code == IR_POWER) {
    autoMode = !autoMode;
    stopMotors();
    lastCommand = autoMode ? "AUTOPILOT" : "STOP";
    updateDisplay();
    Serial.print(F(" -> [MODE CHANGED] Now: "));
    Serial.println(autoMode ? F("AUTO MODE (OBSTACLE AVOIDANCE)") : F("MANUAL MODE"));
    return;
  }

  if (autoMode) {
    Serial.println(F(" -> [IGNORED] System is in AUTO mode"));
    return;
  }

  // MANUAL Mode Controls
  if (code == 0x0E || code == 0x0D || code == 0x18) {
    moveForward();
    lastCommand = "FORWARD";
  } 
  else if (code == 0x1A || code == 0x19 || code == 0x52) {
    moveBackward();
    lastCommand = "BACKWARD";
  } 
  else if (code == 0x0A || code == 0x08 || code == 0x1C) {
    turnLeft();
    lastCommand = "LEFT";
  } 
  else if (code == 0x1E || code == 0x5A || code == 0x0C) {
    turnRight();
    lastCommand = "RIGHT";
  } 
  else if (code == IR_OK || code == 0x1D) {
    stopMotors();
    lastCommand = "STOP";
  } 
  else {
    Serial.println(F(" -> [UNKNOWN BUTTON]"));
    return;
  }

  Serial.print(F(" -> Action: "));
  Serial.println(lastCommand);
  updateDisplay();
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M_EN, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MOUT1, OUTPUT);
  stopMotors();

  u8g2.begin();
  IrReceiver.begin(TSOP_PIN, DISABLE_LED_FEEDBACK);

  Serial.println(F("========================================"));
  Serial.println(F("  DAY 39: PRACTICAL-77 HYBRID CONTROLLER "));
  Serial.println(F("========================================"));
  Serial.println(F("Press 'POWER' (0x09) on IR Remote to toggle AUTO/MANUAL"));
  Serial.println(F("MANUAL Controls: UP / DOWN / LEFT / RIGHT / OK"));
  Serial.println(F("Default Mode: MANUAL | Motors: STOPPED"));

  updateDisplay();
}

//-------------------------------------------
void loop() {
  // 1. IR RECEIVE & DECODE
  if (IrReceiver.decode()) {
    byte code = IrReceiver.decodedIRData.command;
    if (code != 0x00) {
      handleIRCommand(code);
    }
    IrReceiver.resume();
  }

  // 2. AUTO MODE LOGIC (Obstacle Avoidance)
  if (autoMode) {
    if (millis() - lastSensorTime >= 80) {
      lastSensorTime = millis();
      distance = readDistance();

      if (distance > 0 && distance <= OBSTACLE_DIST) {
        turnRight();
        lastCommand = "AUTO: TURN";
      } else {
        moveForward();
        lastCommand = "AUTO: FWD";
      }
      updateDisplay();
    }
  }

  // 3. PERIODIC SERIAL TELEMETRY (Every 600ms)
  if (millis() - lastSerialTime >= 600) {
    lastSerialTime = millis();
    Serial.print(F("SYS | Mode: "));
    Serial.print(autoMode ? F("AUTO  ") : F("MANUAL"));
    Serial.print(F(" | Action: "));
    Serial.print(lastCommand);
    for (byte i = strlen(lastCommand); i < 11; i++) Serial.print(' ');
    Serial.print(F(" | Dist: "));
    if (distance >= 400 || distance <= 0 || distance == 999) Serial.println(F("--"));
    else { Serial.print(distance); Serial.println(F(" cm")); }
  }
}