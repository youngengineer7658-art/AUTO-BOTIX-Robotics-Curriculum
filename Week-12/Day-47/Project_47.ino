/*
==================================================
Day 47 | Project-47: Final Autonomous Robot
Board  : Young Engineer Board
Pinout:
  - Motors   : Left (D2, D4), Right (D7, D8)
  - Sensors  : Ultrasonic (Trig D5, Echo A3), IR (A0)
  - UI       : Buttons (A6), OLED Display (I2C)
  - Serial   : 9600 Baud
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define MOTOR_L_FWD 2
#define MOTOR_L_REV 4
#define MOTOR_R_FWD 7
#define MOTOR_R_REV 8

#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_PIN   A0
#define BTN_PIN  A6

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

enum RobotState { STATE_IDLE, STATE_RUNNING };
RobotState robotMode = STATE_IDLE;

enum MotionCmd { CMD_STOP, CMD_FWD, CMD_REV, CMD_RIGHT, CMD_LEFT };
MotionCmd activeCmd = CMD_STOP;

int distanceCm = 0;
bool irObstacle = false;

// Non-blocking timers
unsigned long lastSenseTime = 0;
unsigned long lastBtnDebounce = 0;
Button lastBtnState = BTN_NONE;

void driveStop() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  activeCmd = CMD_STOP;
}

void driveForward() {
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  digitalWrite(MOTOR_L_FWD, HIGH);
  digitalWrite(MOTOR_R_FWD, HIGH);
  activeCmd = CMD_FWD;
}

void driveReverse() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_L_REV, HIGH);
  digitalWrite(MOTOR_R_REV, HIGH);
  activeCmd = CMD_REV;
}

void driveTurnRight() {
  digitalWrite(MOTOR_L_REV, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  digitalWrite(MOTOR_L_FWD, HIGH);
  digitalWrite(MOTOR_R_REV, HIGH);
  activeCmd = CMD_RIGHT;
}

void driveTurnLeft() {
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_R_REV, LOW);
  digitalWrite(MOTOR_L_REV, HIGH);
  digitalWrite(MOTOR_R_FWD, HIGH);
  activeCmd = CMD_LEFT;
}

int readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 22000UL);
  if (duration == 0 || duration > 20000UL) return -1;
  return (int)((duration * 0.034) / 2);
}

void updateDashboard() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "P-47: AUTONOMOUS BOT");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 30, "SYS :");
    u8g2.drawStr(55, 30, (robotMode == STATE_RUNNING) ? "AUTONOMOUS" : "STANDBY/STOP");

    u8g2.drawStr(5, 45, "CMD :");
    switch (activeCmd) {
      case CMD_FWD:   u8g2.drawStr(55, 45, "FORWARD"); break;
      case CMD_REV:   u8g2.drawStr(55, 45, "REVERSE"); break;
      case CMD_RIGHT: u8g2.drawStr(55, 45, "TURN R");  break;
      case CMD_LEFT:  u8g2.drawStr(55, 45, "TURN L");  break;
      default:        u8g2.drawStr(55, 45, "STOPPED"); break;
    }

    u8g2.drawStr(5, 60, "US:");
    if (distanceCm == -1) {
      u8g2.drawStr(30, 60, "--");
    } else {
      char b[5]; itoa(distanceCm, b, 10);
      u8g2.drawStr(30, 60, b);
      u8g2.drawStr(50, 60, "cm");
    }

    u8g2.drawStr(75, 60, "IR:");
    u8g2.drawStr(95, 60, irObstacle ? "HIT" : "OK");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_L_FWD, OUTPUT);
  pinMode(MOTOR_L_REV, OUTPUT);
  pinMode(MOTOR_R_FWD, OUTPUT);
  pinMode(MOTOR_R_REV, OUTPUT);
  driveStop();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  u8g2.begin();
  Serial.println(F("PROJECT-47: FINAL AUTONOMOUS BOT INITIALIZED"));
  updateDashboard();
}

void loop() {
  unsigned long now = millis();

  // Button State Machine
  Button btn = readButton();
  if (btn != lastBtnState && (now - lastBtnDebounce > 200)) {
    lastBtnDebounce = now;
    if (btn == BTN_SELECT) {
      robotMode = (robotMode == STATE_RUNNING) ? STATE_IDLE : STATE_RUNNING;
      if (robotMode == STATE_IDLE) driveStop();
      Serial.print(F("[MODE] Switched: "));
      Serial.println(robotMode == STATE_RUNNING ? F("AUTONOMOUS") : F("STANDBY"));
    } else if (btn == BTN_BACK) {
      robotMode = STATE_IDLE;
      driveStop();
      Serial.println(F("[E-STOP] Emergency motor cutoff"));
    }
  }
  lastBtnState = btn;

  // Autonomous Control Task (every 80ms)
  if (now - lastSenseTime >= 80) {
    lastSenseTime = now;
    distanceCm = readUltrasonic();
    irObstacle = (analogRead(IR_PIN) < 500);

    if (robotMode == STATE_RUNNING) {
      if (irObstacle || (distanceCm > 0 && distanceCm < 12)) {
        // Immediate wall avoidance: Reverse briefly then Pivot Left
        driveReverse();
        delay(180);
        driveTurnLeft();
        delay(220);
      } else if (distanceCm >= 12 && distanceCm <= 25) {
        // Warning zone: Steer Right
        driveTurnRight();
        delay(160);
      } else {
        // Clear road ahead
        driveForward();
      }
    } else {
      driveStop();
    }

    updateDashboard();

    Serial.print(F("Mode: "));
    Serial.print(robotMode == STATE_RUNNING ? F("RUN") : F("STOP"));
    Serial.print(F(" | Cmd: "));
    Serial.print((int)activeCmd);
    Serial.print(F(" | Dist: "));
    Serial.print(distanceCm);
    Serial.print(F("cm | IR: "));
    Serial.println(irObstacle ? F("BLOCKED") : F("CLEAR"));
  }
}
