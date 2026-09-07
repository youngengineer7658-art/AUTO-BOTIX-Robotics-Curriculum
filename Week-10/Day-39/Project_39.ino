/*
==================================================
Day 39 | Project-39: Hybrid Robot
Board  : Young Engineer Board / Auto-Botix (Schematic Verified)
Hardware:
  - Motors: L293D (M11: 2, M12: 4, EN1: 6, M21: 7, M22: 8, EN2: 9)
  - Remote: TSOP IR Receiver on Pin D3
  - Override: K1 Push Button on Pin A6 Resistor Ladder
  - Obstacle: Ultrasonic Sensor (Trig: 5, Echo: A3)
  - Visual: WS2812B RGB NeoPixel on Pin D11 + 0.96" OLED (U8g2)
==================================================
*/

#define DECODE_NEC
#include <Wire.h>
#include <U8g2lib.h>
#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

// 1. PIN DEFINITIONS (Schematic Matched)
#define TSOP_PIN     3
#define TRIG_PIN     5
#define ECHO_PIN     A3
#define BUTTONS_PIN  A6
#define RGB_PIN      11
#define NUM_LEDS     1

// L293D Pins
#define M11          2
#define M12          4
#define M_EN         6   // Motor 1 Speed PWM
#define M21          7
#define M22          8
#define MOUT1        9   // Motor 2 Speed PWM

// 2. HARDWARE OBJECTS (RAM-Safe U8g2 & NeoPixel)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel rgbLed(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// 3. PARAMETERS & COMMANDS
#define IR_POWER     0x09
#define IR_OK        0x05

const int MOTOR_SPEED   = 220; // High driving torque
const int OBSTACLE_DIST = 20;  // Threshold in cm

// 4. SYSTEM STATES
bool autoMode = false;
const char* lastCommand = "STOP";
int currentDist = 999;
bool obstacleDetected = false;

// K1 Button Debounce Tracking
bool lastK1State = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
// RGB LED FEEDBACK
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
  rgbLed.show();
}

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
// SENSORS & OVERRIDE BUTTON
int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

bool isK1Pressed() {
  int adcVal = analogRead(BUTTONS_PIN);
  return (adcVal >= 950);
}

void checkOverrideButton() {
  bool currentK1State = isK1Pressed();

  if (currentK1State && !lastK1State && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    autoMode = !autoMode;
    stopMotors();
    lastCommand = autoMode ? "AUTO ACTIVE" : "MANUAL OVR";
    setRGB(autoMode ? 0 : 0, autoMode ? 255 : 0, autoMode ? 0 : 255); // Green for Auto, Blue for Manual
    updateDisplay();
    Serial.print(F("[K1 OVERRIDE] Mode Switched -> "));
    Serial.println(autoMode ? F("AUTO") : F("MANUAL"));
  }
  lastK1State = currentK1State;
}

//-------------------------------------------
// OLED DASHBOARD
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(14, 12, "HYBRID ROBOT");
    u8g2.drawHLine(0, 15, 128);

    // Mode
    u8g2.drawStr(5, 30, "MODE :");
    u8g2.drawStr(55, 30, autoMode ? "AUTONOMOUS" : "MANUAL");

    // Action
    u8g2.drawStr(5, 45, "ACT  :");
    u8g2.drawStr(55, 45, lastCommand);

    // Live Distance / Obstacle State
    u8g2.drawStr(5, 60, "DIST :");
    if (currentDist == 999 || currentDist <= 0) {
      u8g2.drawStr(55, 60, "-- cm");
    } else {
      char buf[6];
      itoa(currentDist, buf, 10);
      u8g2.drawStr(55, 60, buf);
      u8g2.drawStr(80, 60, "cm");
      if (obstacleDetected) u8g2.drawStr(100, 60, "[!]");
    }
  } while (u8g2.nextPage());
}

//-------------------------------------------
// IR COMMAND PROCESSOR
void handleIRCommand(byte code) {
  if (code == IR_POWER) {
    autoMode = !autoMode;
    stopMotors();
    lastCommand = autoMode ? "AUTOPILOT" : "STOP";
    setRGB(autoMode ? 0 : 0, autoMode ? 255 : 0, autoMode ? 0 : 255);
    updateDisplay();
    Serial.print(F("[IR POWER] Mode: "));
    Serial.println(autoMode ? F("AUTO") : F("MANUAL"));
    return;
  }

  if (autoMode) return; // Directional keys ignored in AUTO mode

  if (code == 0x0E || code == 0x0D || code == 0x18) {
    moveForward();
    lastCommand = "FORWARD";
    setRGB(0, 255, 0); // Green
  } 
  else if (code == 0x1A || code == 0x19 || code == 0x52) {
    moveBackward();
    lastCommand = "BACKWARD";
    setRGB(255, 0, 0); // Red
  } 
  else if (code == 0x0A || code == 0x08 || code == 0x1C) {
    turnLeft();
    lastCommand = "LEFT";
    setRGB(255, 255, 0); // Yellow
  } 
  else if (code == 0x1E || code == 0x5A || code == 0x0C) {
    turnRight();
    lastCommand = "RIGHT";
    setRGB(0, 255, 255); // Cyan
  } 
  else if (code == IR_OK || code == 0x1D) {
    stopMotors();
    lastCommand = "STOP";
    setRGB(0, 0, 255); // Blue
  } 
  else {
    return;
  }

  updateDisplay();
}

//-------------------------------------------
// AUTONOMOUS NAVIGATION ESCAPE MANEUVER
void executeAutoAvoidance() {
  obstacleDetected = true;
  setRGB(255, 0, 0); // Red Warning

  stopMotors();
  lastCommand = "AUTO: STOP";
  updateDisplay();
  delay(150);

  // Short reverse back
  lastCommand = "AUTO: BACK";
  updateDisplay();
  moveBackward();
  delay(700);

  // Steering escape turn
  lastCommand = "AUTO: TURN";
  updateDisplay();
  turnRight();
  delay(550);

  stopMotors();
  obstacleDetected = false;
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

  rgbLed.begin();
  setRGB(0, 0, 255); // Default Manual Standby (Blue)

  u8g2.begin();
  IrReceiver.begin(TSOP_PIN, DISABLE_LED_FEEDBACK);

  Serial.println(F("========================================"));
  Serial.println(F("   DAY 39: PROJECT-39 HYBRID ROBOT      "));
  Serial.println(F("========================================"));
  updateDisplay();
}

//-------------------------------------------
void loop() {
  // 1. HARDWARE BUTTON OVERRIDE (Instant Priority)
  checkOverrideButton();

  // 2. IR REMOTE RECEPTION
  if (IrReceiver.decode()) {
    byte code = IrReceiver.decodedIRData.command;
    if (code != 0x00) {
      handleIRCommand(code);
    }
    IrReceiver.resume();
  }

  // 3. AUTONOMOUS OBSTACLE NAVIGATION
  if (autoMode) {
    if (millis() - lastSensorTime >= 70) {
      lastSensorTime = millis();
      currentDist = getDistanceCM();

      if (currentDist > 0 && currentDist <= OBSTACLE_DIST) {
        executeAutoAvoidance();
      } else {
        moveForward();
        setRGB(0, 255, 0); // Solid Green when clear
        lastCommand = "AUTO: RUN";
      }
      updateDisplay();
    }
  }

  // 4. PERIODIC SERIAL MONITOR LOGGING
  if (millis() - lastSerialTime >= 600) {
    lastSerialTime = millis();
    Serial.print(F("SysMode: "));
    Serial.print(autoMode ? F("AUTO  ") : F("MANUAL"));
    Serial.print(F(" | Command: "));
    Serial.print(lastCommand);
    Serial.print(F(" | Obstacle: "));
    if (currentDist == 999) Serial.println(F("CLEAR"));
    else { Serial.print(currentDist); Serial.println(F(" cm")); }
  }
}
