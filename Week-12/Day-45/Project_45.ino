/*
==================================================
Day 45 | Project-45: Smart Robot Base (Serial Telemetry Added)
Board  : Young Engineer Board
Pinout:
  - Motors  : L_FWD D6 | L_REV D7 | R_FWD D8 | R_REV D9
  - Sensors : Trig D5 | Echo A3 | IR A0
  - UI      : Buttons A6 | OLED I2C (U8g2)
  - Serial  : Baud 9600 (Live telemetry stream)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// Motor Driver Pins
#define MOTOR_L_FWD 6
#define MOTOR_L_REV 7
#define MOTOR_R_FWD 8
#define MOTOR_R_REV 9

// Sensor & Button Pins
#define TRIG_PIN    5
#define ECHO_PIN    A3
#define IR_PIN      A0
#define BTN_PIN     A6

// Calibrated Ladder Ranges (Pin A6)
#define BTN_SEL_MIN  925
#define BTN_SEL_MAX  938
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

enum Button { BTN_NONE, BTN_SELECT, BTN_BACK };

Button readButton() {
  int val = analogRead(BTN_PIN);
  if (val >= BTN_SEL_MIN  && val <= BTN_SEL_MAX)  return BTN_SELECT;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

// RAM-Safe U8g2 Constructor
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// System & Motion States
enum BotMode { MODE_STOPPED, MODE_RUNNING };
BotMode systemMode = MODE_STOPPED;

enum MotionState { MOTION_STOP, MOTION_FWD, MOTION_REV, MOTION_TURN_LEFT, MOTION_TURN_RIGHT };
MotionState currentMotion = MOTION_STOP;

int distanceCm = 0;
bool irDetected = false;

unsigned long lastCycleTime = 0;
const unsigned long CYCLE_INTERVAL = 80;

Button lastBtn = BTN_NONE;
unsigned long lastDebounce = 0;

void motorStop() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  currentMotion = MOTION_STOP;
}

void motorForward() {
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  digitalWrite(MOTOR_L_FWD, HIGH);
  digitalWrite(MOTOR_R_FWD, HIGH);
  currentMotion = MOTION_FWD;
}

void motorReverse() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_L_REV, HIGH);
  digitalWrite(MOTOR_R_REV, HIGH);
  currentMotion = MOTION_REV;
}

void motorTurnRight() {
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_L_FWD, HIGH);
  digitalWrite(MOTOR_R_REV, HIGH);
  currentMotion = MOTION_TURN_RIGHT;
}

void motorTurnLeft() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  digitalWrite(MOTOR_L_REV, HIGH);
  digitalWrite(MOTOR_R_FWD, HIGH);
  currentMotion = MOTION_TURN_LEFT;
}

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0 || duration > 25000UL) return -1;
  int cm = (int)((duration * 0.034) / 2);
  if (cm < 2 || cm > 350) return -1;
  return cm;
}

void readIR() {
  int raw = analogRead(IR_PIN);
  irDetected = (raw < 500);
}

void printSerialTelemetry() {
  Serial.print(F("[ROBOT] Mode: "));
  Serial.print(systemMode == MODE_RUNNING ? F("RUNNING") : F("STOPPED"));
  
  Serial.print(F(" | Motion: "));
  switch (currentMotion) {
    case MOTION_FWD:        Serial.print(F("FORWARD"));     break;
    case MOTION_REV:        Serial.print(F("REVERSING"));   break;
    case MOTION_TURN_RIGHT: Serial.print(F("TURN RIGHT"));  break;
    case MOTION_TURN_LEFT:  Serial.print(F("TURN LEFT"));   break;
    default:                Serial.print(F("STOPPED"));     break;
  }

  Serial.print(F(" | US Dist: "));
  if (distanceCm == -1) Serial.print(F("-- cm"));
  else { Serial.print(distanceCm); Serial.print(F(" cm")); }

  Serial.print(F(" | IR A0: "));
  Serial.println(irDetected ? F("OBJECT_DETECTED") : F("CLEAR"));
}

void updateOLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PROJECT-45: ROBOT");
    u8g2.drawHLine(0, 15, 128);

    // Run Mode
    u8g2.drawStr(5, 30, "MODE  :");
    u8g2.drawStr(60, 30, (systemMode == MODE_RUNNING) ? "RUNNING" : "STOP/E-STOP");

    // Motion State
    u8g2.drawStr(5, 45, "MOVE  :");
    switch (currentMotion) {
      case MOTION_FWD:        u8g2.drawStr(60, 45, "FORWARD");     break;
      case MOTION_REV:        u8g2.drawStr(60, 45, "REVERSING");   break;
      case MOTION_TURN_RIGHT: u8g2.drawStr(60, 45, "TURN RIGHT");  break;
      case MOTION_TURN_LEFT:  u8g2.drawStr(60, 45, "TURN LEFT");   break;
      default:                u8g2.drawStr(60, 45, "STOPPED");     break;
    }

    // Telemetry
    u8g2.drawStr(5, 60, "US:");
    if (distanceCm == -1) {
      u8g2.drawStr(30, 60, "--");
    } else {
      char dBuf[5]; itoa(distanceCm, dBuf, 10);
      u8g2.drawStr(30, 60, dBuf);
      u8g2.drawStr(50, 60, "cm");
    }

    u8g2.drawStr(75, 60, "IR:");
    u8g2.drawStr(95, 60, irDetected ? "OBJ" : "OK");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(MOTOR_L_FWD, OUTPUT);
  pinMode(MOTOR_L_REV, OUTPUT);
  pinMode(MOTOR_R_FWD, OUTPUT);
  pinMode(MOTOR_R_REV, OUTPUT);
  motorStop();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" PROJECT-45: SMART ROBOT BASE (SERIAL) "));
  Serial.println(F("========================================"));
  Serial.println(F("Commands: [SELECT: Start/Pause] | [BACK: Emergency Stop]"));
  updateOLED();
}

void loop() {
  // Button Inputs
  Button btn = readButton();
  if (btn != lastBtn && (millis() - lastDebounce > 200)) {
    lastDebounce = millis();
    if (btn == BTN_SELECT) {
      systemMode = (systemMode == MODE_RUNNING) ? MODE_STOPPED : MODE_RUNNING;
      if (systemMode == MODE_STOPPED) motorStop();
      Serial.print(F("[EVENT] Select Pressed -> System: "));
      Serial.println(systemMode == MODE_RUNNING ? F("STARTED") : F("PAUSED"));
    } else if (btn == BTN_BACK) {
      systemMode = MODE_STOPPED;
      motorStop();
      Serial.println(F("[EVENT] Back Pressed -> EMERGENCY E-STOP CUTOFF!"));
    }
  }
  lastBtn = btn;

  // Sensor reading & Navigation loop
  unsigned long currentMillis = millis();
  if (currentMillis - lastCycleTime >= CYCLE_INTERVAL) {
    lastCycleTime = currentMillis;

    distanceCm = readDistance();
    readIR();

    if (systemMode == MODE_RUNNING) {
      if (irDetected || (distanceCm > 0 && distanceCm < 12)) {
        motorReverse();
        delay(200);
        motorTurnLeft();
        delay(250);
      } else if (distanceCm >= 12 && distanceCm <= 25) {
        motorTurnRight();
        delay(200);
      } else {
        motorForward();
      }
    } else {
      motorStop();
    }

    // Stream status to Serial Monitor & OLED
    printSerialTelemetry();
    updateOLED();
  }
}
