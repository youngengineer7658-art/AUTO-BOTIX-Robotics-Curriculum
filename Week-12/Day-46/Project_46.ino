/*
==================================================
Day 46 | Project-46: Smooth Car Movement
Board  : Young Engineer Board
Pinout:
  - Motors   : L_FWD D2, L_REV D4, R_FWD D7, R_REV D8
  - Sensors  : Ultrasonic (Trig D5, Echo A3), IR Sensor (A0)
  - UI       : Buttons (A6), OLED Display (I2C)
  - Serial   : 9600 Baud
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

// Updated Motor Pins
#define MOTOR_L_FWD 2
#define MOTOR_L_REV 4
#define MOTOR_R_FWD 7
#define MOTOR_R_REV 8

#define TRIG_PIN    5
#define ECHO_PIN    A3
#define IR_PIN      A0
#define BTN_PIN     A6

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

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

enum SystemState { STATE_STOPPED, STATE_RUNNING };
SystemState robotState = STATE_STOPPED;

// Speed Ramping Parameters (Software PWM)
int currentSpeed = 0;
int targetSpeed  = 0;
const int MAX_SPEED = 200;
const int RAMP_STEP = 10;

int distanceCm = 0;
bool irDetected = false;

unsigned long prevSense = 0;
unsigned long prevRamp  = 0;
unsigned long prevDebounce = 0;
Button prevBtn = BTN_NONE;

void setMotorDrive(int speed) {
  if (speed <= 0) {
    digitalWrite(MOTOR_L_FWD, LOW);
    digitalWrite(MOTOR_L_REV, LOW);
    digitalWrite(MOTOR_R_FWD, LOW);
    digitalWrite(MOTOR_R_REV, LOW);
    return;
  }
  
  // Software PWM drive on non-PWM pins
  int onTime = map(speed, 0, MAX_SPEED, 0, 10);
  int offTime = 10 - onTime;

  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_REV, LOW);

  if (onTime > 0) {
    digitalWrite(MOTOR_L_FWD, HIGH);
    digitalWrite(MOTOR_R_FWD, HIGH);
    delayMicroseconds(onTime * 100);
  }
  if (offTime > 0) {
    digitalWrite(MOTOR_L_FWD, LOW);
    digitalWrite(MOTOR_R_FWD, LOW);
    delayMicroseconds(offTime * 100);
  }
}

void eStop() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  currentSpeed = 0;
  targetSpeed = 0;
}

void rampSpeed() {
  if (currentSpeed < targetSpeed) {
    currentSpeed += RAMP_STEP;
    if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
  } else if (currentSpeed > targetSpeed) {
    currentSpeed -= RAMP_STEP;
    if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
  }
}

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 20000UL);
  if (duration == 0 || duration > 18000UL) return -1;
  return (int)((duration * 0.034) / 2);
}

void updateOLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PROJECT-46: SMOOTH");
    u8g2.drawHLine(0, 15, 128);

    // State
    u8g2.drawStr(5, 30, "STATE :");
    u8g2.drawStr(60, 30, (robotState == STATE_RUNNING) ? "RUNNING" : "E-STOP");

    // Speed %
    u8g2.drawStr(5, 45, "SPEED :");
    int pct = map(currentSpeed, 0, MAX_SPEED, 0, 100);
    char sBuf[5]; itoa(pct, sBuf, 10);
    u8g2.drawStr(60, 45, sBuf);
    u8g2.drawStr(90, 45, "%");

    // Sensors
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

  pinMode(MOTOR_L_FWD, OUTPUT);
  pinMode(MOTOR_L_REV, OUTPUT);
  pinMode(MOTOR_R_FWD, OUTPUT);
  pinMode(MOTOR_R_REV, OUTPUT);
  eStop();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();
  Serial.println(F("PROJECT-46 READY (D2, D4, D7, D8)"));
}

void loop() {
  unsigned long now = millis();

  // Button Input
  Button btn = readButton();
  if (btn != prevBtn && (now - prevDebounce > 200)) {
    prevDebounce = now;
    if (btn == BTN_SELECT) {
      robotState = (robotState == STATE_RUNNING) ? STATE_STOPPED : STATE_RUNNING;
      if (robotState == STATE_STOPPED) eStop();
    } else if (btn == BTN_BACK) {
      robotState = STATE_STOPPED;
      eStop();
      Serial.println(F("[E-STOP] Stopped"));
    }
  }
  prevBtn = btn;

  // Sensor Evaluation (Every 100ms)
  if (now - prevSense >= 100) {
    prevSense = now;
    distanceCm = readDistance();
    irDetected = (analogRead(IR_PIN) < 500);

    if (robotState == STATE_RUNNING) {
      if (irDetected || (distanceCm > 0 && distanceCm < 15)) {
        targetSpeed = 0;             // Smooth brake
      } else if (distanceCm >= 15 && distanceCm < 30) {
        targetSpeed = MAX_SPEED / 2; // Slow cruise
      } else {
        targetSpeed = MAX_SPEED;     // Full speed
      }
    } else {
      targetSpeed = 0;
    }

    updateOLED();

    // Serial Telemetry
    Serial.print(F("Speed: "));
    Serial.print(currentSpeed);
    Serial.print(F(" | US: "));
    Serial.print(distanceCm);
    Serial.print(F("cm | IR: "));
    Serial.println(irDetected ? F("OBJ") : F("OK"));
  }

  // Smooth Speed Ramp (Every 20ms)
  if (now - prevRamp >= 20) {
    prevRamp = now;
    if (robotState == STATE_RUNNING || currentSpeed > 0) {
      rampSpeed();
    }
  }

  // Motor Drive Pulse
  if (robotState == STATE_RUNNING && currentSpeed > 0) {
    setMotorDrive(currentSpeed);
  } else {
    eStop();
  }
}
