/*
==================================================
Day 39 | Practical-78: Add Override Button
Board  : Young Engineer Board / Auto-Botix (Schematic Verified)
Hardware:
  - Override Button: K1 Onboard Button (Pin A6 Resistor Ladder)
  - IR Receiver: TSOP (Pin D3)
  - Motors: L293D (M11: 2, M12: 4, EN1: 6, M21: 7, M22: 8, EN2: 9)
  - Ultrasonic Sensor: Trig (Pin 5), Echo (Pin A3)
  - Display: 0.96" I2C OLED (U8g2 - RAM Safe)
==================================================
*/

#define DECODE_NEC
#include <Wire.h>
#include <U8g2lib.h>
#include <IRremote.hpp>

// 1. PIN DEFINITIONS
#define TSOP_PIN     3   // IR Receiver
#define TRIG_PIN     5   // Ultrasonic Trig
#define ECHO_PIN     A3  // Ultrasonic Echo
#define BUTTONS_PIN  A6  // Resistor Ladder Buttons (K1 Override)

// L293D Motor Pins
#define M11          2   // Motor 1 In 1
#define M12          4   // Motor 1 In 2
#define M_EN         6   // Motor 1 Speed PWM
#define M21          7   // Motor 2 In 1
#define M22          8   // Motor 2 In 2
#define MOUT1        9   // Motor 2 Speed PWM

// 2. OLED DISPLAY (128 bytes RAM)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 3. PARAMETERS & COMMANDS
#define IR_POWER     0x09
#define IR_OK        0x05

const int MOTOR_SPEED   = 220; // Driving torque
const int OBSTACLE_DIST = 25;  // Obstacle threshold (cm)

// 4. SYSTEM STATES
bool autoMode = false;
const char* lastCommand = "STOP";
int distance = 999;

// K1 Button Debounce Tracking
bool lastK1State = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
// MOTOR CONTROL FUNCTIONS
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
// SENSOR & BUTTON LOGIC
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

// Check K1 Override Button on A6
bool isK1Pressed() {
  int adcVal = analogRead(BUTTONS_PIN);
  return (adcVal >= 950); // K1 connects VCC to A6
}

void checkOverrideButton() {
  bool currentK1State = isK1Pressed();

  if (currentK1State && !lastK1State && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    autoMode = !autoMode;
    stopMotors();
    lastCommand = autoMode ? "AUTO (OVR)" : "MANUAL (OVR)";
    updateDisplay();
    Serial.print(F("[K1 OVERRIDE] Mode switched to: "));
    Serial.println(autoMode ? F("AUTO") : F("MANUAL"));
  }
  lastK1State = currentK1State;
}

//-------------------------------------------
// OLED SCREEN DISPLAY
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "ADD OVERRIDE BTN");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 32, "MODE  :");
    u8g2.drawStr(60, 32, autoMode ? "AUTO" : "MANUAL");

    u8g2.drawStr(5, 48, "CMD   :");
    u8g2.drawStr(60, 48, lastCommand);

    u8g2.drawStr(5, 62, "BTN: K1 OVERRIDE");
  } while (u8g2.nextPage());
}

//-------------------------------------------
// IR REMOTE LOGIC
void handleIRCommand(byte code) {
  if (code == IR_POWER) {
    autoMode = !autoMode;
    stopMotors();
    lastCommand = autoMode ? "AUTO ACTIVE" : "MANUAL";
    updateDisplay();
    Serial.print(F("[IR] Power Pressed! Mode: "));
    Serial.println(autoMode ? F("AUTO") : F("MANUAL"));
    return;
  }

  if (autoMode) return; // Ignore movement commands in AUTO mode

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
    return;
  }

  updateDisplay();
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTONS_PIN, INPUT);

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
  Serial.println(F(" PRACTICAL-78: ADD OVERRIDE BUTTON      "));
  Serial.println(F("========================================"));
  updateDisplay();
}

//-------------------------------------------
void loop() {
  // 1. HARDWARE K1 OVERRIDE BUTTON CHECK (Always active)
  checkOverrideButton();

  // 2. IR REMOTE RECEPTION
  if (IrReceiver.decode()) {
    byte code = IrReceiver.decodedIRData.command;
    if (code != 0x00) {
      handleIRCommand(code);
    }
    IrReceiver.resume();
  }

  // 3. AUTONOMOUS OBSTACLE AVOIDANCE (Runs only in Auto Mode)
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

  // 4. PERIODIC SERIAL MONITOR TELEMETRY
  if (millis() - lastSerialTime >= 600) {
    lastSerialTime = millis();
    Serial.print(F("Status | Mode: "));
    Serial.print(autoMode ? F("AUTO  ") : F("MANUAL"));
    Serial.print(F(" | Action: "));
    Serial.print(lastCommand);
    Serial.print(F(" | Obstacle: "));
    if (distance == 999) Serial.println(F("--"));
    else { Serial.print(distance); Serial.println(F(" cm")); }
  }
}