/*
==================================================
Day 38 | Project-38: Autonomous Path Navigator (Fixed Motors)
Board  : Young Engineer Board / Auto-Botix
Hardware: Ultrasonic (D5, A3) + Left IR (A0) + Right IR (A1)
          Motors: L_DIR (D7), L_PWM (D6), R_DIR (D8), R_PWM (D9)
          Display: OLED (U8g2)
          RGB NeoPixel (D11) + Buzzer (D12)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

// 1. PIN DEFINITIONS
#define TRIG_PIN     5
#define ECHO_PIN     A3
#define LEFT_IR_PIN  A0
#define RIGHT_IR_PIN A1

// Motor Driver Pins
#define MOTOR_L_PWM  6  // Left Motor Speed
#define MOTOR_L_DIR  7  // Left Motor Direction
#define MOTOR_R_DIR  8  // Right Motor Direction
#define MOTOR_R_PWM  9  // Right Motor Speed

#define RGB_PIN      11
#define NUM_LEDS     8
#define BUZZER_PIN   12

// 2. HARDWARE OBJECTS
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel rgb(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// 3. PARAMETERS & SPEED (Full Speed PWM to prevent motor stall)
const int FRONT_OBSTACLE_DIST = 25; // cm
const int IR_OBSTACLE_ADC     = 500; // ADC > 500 = Obstacle
const int DRIVE_SPEED         = 230; // 0 to 255 (High torque)
const int TURN_SPEED          = 220;

// 4. VARIABLES
int frontDistance = 999;
bool leftBlocked = false;
bool rightBlocked = false;
bool frontBlocked = false;
const char* statusText = "FORWARD";

unsigned long lastSensorTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastSerialTime = 0;

//-------------------------------------------
void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    rgb.setPixelColor(i, rgb.Color(r, g, b));
  }
  rgb.show();
}

void beep(int ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(ms);
  digitalWrite(BUZZER_PIN, LOW);
}

//-------------------------------------------
// MOTOR CONTROL FUNCTIONS (Proper polarity for both sides)
void motorStop() {
  analogWrite(MOTOR_L_PWM, 0);
  analogWrite(MOTOR_R_PWM, 0);
  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_R_DIR, LOW);
}

void driveForward() {
  // Dono motors forward
  digitalWrite(MOTOR_L_DIR, HIGH);
  analogWrite(MOTOR_L_PWM, DRIVE_SPEED);
  
  digitalWrite(MOTOR_R_DIR, HIGH);
  analogWrite(MOTOR_R_PWM, DRIVE_SPEED);
  
  setRGBColor(0, 255, 0); // Green
  statusText = "FORWARD";
}

void turnLeft() {
  // Left Motor Reverse, Right Motor Forward
  digitalWrite(MOTOR_L_DIR, LOW);
  analogWrite(MOTOR_L_PWM, TURN_SPEED);
  
  digitalWrite(MOTOR_R_DIR, HIGH);
  analogWrite(MOTOR_R_PWM, TURN_SPEED);
  
  setRGBColor(0, 0, 255); // Blue
  statusText = "TURN LEFT";
}

void turnRight() {
  // Left Motor Forward, Right Motor Reverse
  digitalWrite(MOTOR_L_DIR, HIGH);
  analogWrite(MOTOR_L_PWM, TURN_SPEED);
  
  digitalWrite(MOTOR_R_DIR, LOW);
  analogWrite(MOTOR_R_PWM, TURN_SPEED);
  
  setRGBColor(255, 255, 0); // Yellow
  statusText = "TURN RIGHT";
}

void reverseTurn() {
  // Dono Motors Reverse
  digitalWrite(MOTOR_L_DIR, LOW);
  analogWrite(MOTOR_L_PWM, TURN_SPEED);
  
  digitalWrite(MOTOR_R_DIR, LOW);
  analogWrite(MOTOR_R_PWM, TURN_SPEED);
  
  setRGBColor(255, 0, 0); // Red
  statusText = "REVERSING";
  beep(30);
}

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
void navigatePath() {
  frontDistance = readFrontDistance();
  int leftVal = analogRead(LEFT_IR_PIN);
  int rightVal = analogRead(RIGHT_IR_PIN);

  frontBlocked = (frontDistance > 0 && frontDistance <= FRONT_OBSTACLE_DIST);
  leftBlocked  = (leftVal > IR_OBSTACLE_ADC);
  rightBlocked = (rightVal > IR_OBSTACLE_ADC);

  // Decision Tree
  if (!frontBlocked) {
    driveForward();
  } 
  else if (!leftBlocked && rightBlocked) {
    turnLeft();
  } 
  else if (!rightBlocked && leftBlocked) {
    turnRight();
  } 
  else if (!leftBlocked && !rightBlocked) {
    turnRight(); // Default clear side choice
  } 
  else {
    reverseTurn();
  }
}

//-------------------------------------------
void updateOLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 10, "PATH NAVIGATOR");
    u8g2.drawHLine(0, 13, 128);

    // Distance
    u8g2.drawStr(5, 26, "DIST :");
    if (frontDistance >= 400 || frontDistance <= 0 || frontDistance == 999) {
      u8g2.drawStr(55, 26, "--");
    } else {
      char buf[8];
      itoa(frontDistance, buf, 10);
      u8g2.drawStr(55, 26, buf);
      u8g2.drawStr(85, 26, "cm");
    }

    // Side Sensors
    u8g2.drawStr(5, 40, "SIDES:");
    u8g2.drawStr(55, 40, leftBlocked ? "L:BLK" : "L:CLR");
    u8g2.drawStr(92, 40, rightBlocked ? "R:BLK" : "R:CLR");

    // Action
    u8g2.drawStr(5, 56, "ACTION:");
    u8g2.drawStr(55, 56, statusText);
  } while (u8g2.nextPage());
}

//-------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(100);

  // Sensor Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);

  // Motor Driver Pins
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_R_DIR, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  motorStop();

  u8g2.begin();
  rgb.begin();
  setRGBColor(0, 255, 0);

  Serial.println(F("AUTONOMOUS NAVIGATOR READY"));
  updateOLED();
}

//-------------------------------------------
void loop() {
  // 1. NAVIGATION CONTROL (Har 80ms par scan aur motor run)
  if (millis() - lastSensorTime >= 80) {
    lastSensorTime = millis();
    navigatePath();
  }

  // 2. DISPLAY UPDATE (Har 250ms)
  if (millis() - lastDisplayTime >= 250) {
    lastDisplayTime = millis();
    updateOLED();
  }

  // 3. SERIAL LOGGING
  if (millis() - lastSerialTime >= 600) {
    lastSerialTime = millis();
    Serial.print(F("Dist: "));
    Serial.print(frontDistance);
    Serial.print(F(" cm | Left: "));
    Serial.print(leftBlocked ? F("B") : F("C"));
    Serial.print(F(" | Right: "));
    Serial.print(rightBlocked ? F("B") : F("C"));
    Serial.print(F(" | Motor: "));
    Serial.println(statusText);
  }
}