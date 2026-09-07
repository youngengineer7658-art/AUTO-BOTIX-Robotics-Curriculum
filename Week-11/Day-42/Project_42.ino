/*
==================================================
Day 47 | Practical-93: Hardware Feature Self-Test
Board  : Young Engineer Board
Pinout : Motors (D2, D4, D7, D8) | US (D5, A3) | IR (A0) | BTN (A6) | OLED (I2C)
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

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

struct TestStatus {
  bool oledOk;
  bool usOk;
  bool irOk;
  bool btnOk;
  bool motorOk;
};

TestStatus diag = {false, false, false, false, false};

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long d = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (d == 0 || d > 22000UL) return -1;
  return (int)((d * 0.034) / 2);
}

void testMotors() {
  // Brief forward tick
  digitalWrite(MOTOR_L_FWD, HIGH);
  digitalWrite(MOTOR_R_FWD, HIGH);
  delay(120);
  digitalWrite(MOTOR_L_FWD, LOW);
  digitalWrite(MOTOR_R_FWD, LOW);
  diag.motorOk = true;
}

void renderDiagnosticUI() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "P-93: SYSTEM TEST");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 28, "US SENSOR :");
    u8g2.drawStr(90, 28, diag.usOk ? "PASS" : "WAIT");

    u8g2.drawStr(5, 40, "IR SENSOR :");
    u8g2.drawStr(90, 40, diag.irOk ? "PASS" : "WAIT");

    u8g2.drawStr(5, 52, "BUTTON A6 :");
    u8g2.drawStr(90, 52, diag.btnOk ? "PASS" : "WAIT");

    u8g2.drawStr(5, 64, "MOTORS    :");
    u8g2.drawStr(90, 64, diag.motorOk ? "PASS" : "TEST");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_L_FWD, OUTPUT);
  pinMode(MOTOR_L_REV, OUTPUT);
  pinMode(MOTOR_R_FWD, OUTPUT);
  pinMode(MOTOR_R_REV, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);

  u8g2.begin();
  diag.oledOk = true;

  Serial.println(F("=== P-93: SELF TEST RUNNING ==="));
  renderDiagnosticUI();
  delay(500);

  testMotors();
}

void loop() {
  int d = getDistance();
  if (d > 0 && d < 400) diag.usOk = true;

  int ir = analogRead(IR_PIN);
  if (ir > 10 && ir < 1015) diag.irOk = true;

  int btnVal = analogRead(BTN_PIN);
  if (btnVal > 100) diag.btnOk = true;

  renderDiagnosticUI();

  Serial.print(F("US: "));
  Serial.print(d);
  Serial.print(F("cm | IR: "));
  Serial.print(ir);
  Serial.print(F(" | BTN: "));
  Serial.println(btnVal);

  delay(200);
}
