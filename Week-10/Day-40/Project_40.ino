/*
==================================================
Day 40 | Project-40: Autonomous Robotics Challenge
Board  : Young Engineer Board / Auto-Botix (Schematic Verified)
Hardware:
  - Motors: L293D (M11: 2, M12: 4, EN1: 6, M21: 7, M22: 8, EN2: 9)
  - Remote: TSOP IR Receiver on Pin D3
  - Override: K1 Push Button on Pin A6 Resistor Ladder
  - Obstacle: Ultrasonic Sensor (Trig: 5, Echo: A3)
  - Visual: WS2812B RGB NeoPixel on Pin D11 + 0.96" OLED (U8g2)
Logic:
  - Dual Mode: IR POWER (0x09) or K1 Button toggles MANUAL <-> AUTO
  - In AUTO: Fast ultrasonic detection, smooth cruise, rapid escape sequence
  - In MANUAL: Remote directional control (UP, DOWN, LEFT, RIGHT, OK)
  - Real-time telemetry to OLED and Serial Monitor
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
#define NUM_LEDS     8

// L293D Motor Pins
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

const int MAX_SPEED     = 220; // Driving cruise speed
const int OBSTACLE_DIST = 20;  // Threshold in cm
const int RAMP_STEP     = 10;  // Non-blocking speed increment

// 4. SYSTEM STATES
bool autoMode = false;
const char* lastCommand = "STOP";
int currentSpeed = 0;
int targetSpeed  = 0;
int currentDist  = 999;
bool obstacleDetected = false;

// K1 Debounce Tracking
bool lastK1State = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;
unsigned long lastRampTime   = 0;

//-------------------------------------------
// RGB LED FEEDBACK
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    rgbLed.setPixelColor(i, rgbLed.Color(r, g, b));
  }
  rgbLed.show();
}

//-------------------------------------------
// MOTOR CONTROL
void setMotorPins(byte m11, byte m12, byte m21, byte m22) {
  digitalWrite(M11, m11); digitalWrite(M12, m12);
  digitalWrite(M21, m21); digitalWrite(M22, m22);
}

void applySpeed(int spd) {
  analogWrite(M_EN, spd);
  analogWrite(MOUT1, spd);
}

void processSpeedRamp() {
  if (millis() - lastRampTime >= 15) {
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

void stopMotors() {
  setMotorPins(LOW, LOW, LOW, LOW);
  targetSpeed  = 0;
  currentSpeed = 0;
  applySpeed(0);
}

void moveForward() {
  setMotorPins(HIGH, LOW, HIGH, LOW);
  targetSpeed = MAX_SPEED;
}

void moveBackward() {
  setMotorPins(LOW, HIGH, LOW, HIGH);
  targetSpeed = MAX_SPEED;
}

void turnLeft() {
  setMotorPins(LOW, HIGH, HIGH, LOW);
  targetSpeed = MAX_SPEED;
}

void turnRight() {
  setMotorPins(HIGH, LOW, LOW, HIGH);
  targetSpeed = MAX_SPEED;
}

//-------------------------------------------
// SENSORS & OVERRIDE
int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 22000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

bool isK1Pressed() {
  int val = analogRead(BUTTONS_PIN);
  return (val >= 950);
}

void toggleMode() {
  autoMode = !autoMode;
  stopMotors();
  lastCommand = autoMode ? "AUTONOMOUS" : "MANUAL";
  setRGB(autoMode ? 0 : 0, autoMode ? 255 : 0, autoMode ? 0 : 255);
  updateDisplay();
  Serial.print(F("[MODE CHANGE] System Mode: "));
  Serial.println(autoMode ? F("AUTONOMOUS") : F("MANUAL"));
}

void checkOverrideButton() {
  bool currentK1State = isK1Pressed();
  if (currentK1State && !lastK1State && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    toggleMode();
  }
  lastK1State = currentK1State;
}

//-------------------------------------------
// OLED DASHBOARD
void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 12, "PROJECT-40 FINAL");
    u8g2.drawHLine(0, 15, 128);

    u8g2.drawStr(5, 30, "MODE :");
    u8g2.drawStr(55, 30, autoMode ? "AUTO BOT" : "MANUAL");

    u8g2.drawStr(5, 45, "ACT  :");
    u8g2.drawStr(55, 45, lastCommand);

    u8g2.drawStr(5, 60, "DIST :");
    if (currentDist >= 999 || currentDist <= 0) {
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
// IR COMMAND HANDLER
void handleIRCommand(byte code) {
  if (code == IR_POWER) {
    toggleMode();
    return;
  }

  if (autoMode) return; // Locked in AUTO mode

  if (code == 0x0E || code == 0x0D || code == 0x18) {
    moveForward();
    lastCommand = "FORWARD";
    setRGB(0, 255, 0);
  } 
  else if (code == 0x1A || code == 0x19 || code == 0x52) {
    moveBackward();
    lastCommand = "BACKWARD";
    setRGB(255, 0, 0);
  } 
  else if (code == 0x0A || code == 0x08 || code == 0x1C) {
    turnLeft();
    lastCommand = "LEFT";
    setRGB(255, 255, 0);
  } 
  else if (code == 0x1E || code == 0x5A || code == 0x0C) {
    turnRight();
    lastCommand = "RIGHT";
    setRGB(0, 255, 255);
  } 
  else if (code == IR_OK || code == 0x1D) {
    stopMotors();
    lastCommand = "STOP";
    setRGB(0, 0, 255);
  } 
  else {
    return;
  }

  updateDisplay();
}

//-------------------------------------------
// AUTONOMOUS ESCAPE MANEUVER
void executeAutoAvoidance() {
  obstacleDetected = true;
  setRGB(255, 0, 0); // Red Warning

  stopMotors();
  lastCommand = "AUTO: STOP";
  updateDisplay();
  delay(150);

  // Quick reverse escape
  setMotorPins(LOW, HIGH, LOW, HIGH);
  applySpeed(200);
  lastCommand = "AUTO: BACK";
  updateDisplay();
  delay(700);

  // Steer clear
  setMotorPins(HIGH, LOW, LOW, HIGH); // Turn Right
  applySpeed(200);
  lastCommand = "AUTO: TURN";
  updateDisplay();
  delay(500);

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
  setRGB(0, 0, 255); // Blue standby

  u8g2.begin();
  IrReceiver.begin(TSOP_PIN, DISABLE_LED_FEEDBACK);

  Serial.println(F("========================================"));
  Serial.println(F(" PROJECT-40: AUTONOMOUS ROBOTICS CHLG   "));
  Serial.println(F("========================================"));
  updateDisplay();
}

//-------------------------------------------
void loop() {
  // 1. HARDWARE K1 OVERRIDE
  checkOverrideButton();

  // 2. SPEED ACCELERATION / DECELERATION
  processSpeedRamp();

  // 3. IR REMOTE DECODE
  if (IrReceiver.decode()) {
    byte code = IrReceiver.decodedIRData.command;
    if (code != 0x00) {
      handleIRCommand(code);
    }
    IrReceiver.resume();
  }

  // 4. AUTONOMOUS NAVIGATION (Active in Auto Mode)
  if (autoMode) {
    if (millis() - lastSensorTime >= 65) {
      lastSensorTime = millis();
      currentDist = getDistanceCM();

      if (currentDist > 0 && currentDist <= OBSTACLE_DIST) {
        executeAutoAvoidance();
      } else {
        moveForward();
        setRGB(0, 255, 0); // Green on clear road
        lastCommand = "AUTO: RUN";
      }
      updateDisplay();
    }
  }

  // 5. SERIAL TELEMETRY (Every 500ms)
  if (millis() - lastSerialTime >= 500) {
    lastSerialTime = millis();
    Serial.print(F("SysMode: "));
    Serial.print(autoMode ? F("AUTONOMOUS") : F("MANUAL    "));
    Serial.print(F(" | Action: "));
    Serial.print(lastCommand);
    Serial.print(F(" | PWM: "));
    Serial.print(currentSpeed);
    Serial.print(F(" | Dist: "));
    if (currentDist >= 999 || currentDist <= 0) Serial.println(F("--"));
    else { Serial.print(currentDist); Serial.println(F(" cm")); }
  }
}
