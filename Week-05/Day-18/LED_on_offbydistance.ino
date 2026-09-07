/*
  AUTO-BOTIX — Practical-92: Reduce Delays
  (Day 46, Week 12: Final Integration)

  Same behavior as Practical-91, but delay() calls are replaced with
  a millis()-based state machine. Why this matters:
    - delay() FREEZES the entire program — no sensor reads, no
      button/IR checks, nothing — for the full duration. In the old
      code, the 5-second reverse meant the robot was "deaf" for 5
      full seconds.
    - With millis()-based timing, the loop() keeps running every
      cycle. Sensors can still be read, buttons still respond, and
      the robot could react to a NEW emergency even mid-maneuver
      (not added here, but now possible because nothing blocks).

  Required Arduino libraries:
    - Adafruit GFX Library
    - Adafruit SSD1306
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= Pin Mapping =================
#define PIN_M11        2
#define PIN_M12        4
#define PIN_TRIG       5
#define PIN_MOTOR1_PWM 6
#define PIN_M21        7
#define PIN_M22        8
#define PIN_MOTOR2_PWM 9
#define PIN_ECHO       A3
#define PIN_IR1        A0
#define PIN_IR2        A1

// ================= OLED =================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= IR sensor settings =================
const bool IR_ACTIVE_LOW = true;
const int  IR_THRESHOLD  = 500;

// ================= Ultrasonic settings =================
const int NUM_SAMPLES           = 7;
const unsigned long PING_GAP_MS = 60;
const int OBSTACLE_DISTANCE_CM  = 15;

// ================= Motor settings =================
const int MOTOR_SPEED           = 180;
const unsigned long BACK_TIME_MS = 5000;
const unsigned long TURN_TIME_MS = 600;
const unsigned long PAUSE_TIME_MS = 300;
const unsigned long GAP_TIME_MS   = 150;

const char* lastCommand = "STOP";

struct SensorReadings {
  long distanceCM;
  int ir1;
  int ir2;
  bool obstacleUltrasonic;
  bool obstacleIR1;
  bool obstacleIR2;
  bool obstacleAny;
};

// ---------- Non-blocking obstacle-avoidance state machine ----------
enum RobotState { STATE_DRIVE, STATE_STOPPED, STATE_REVERSING, STATE_TURNING };
RobotState currentState = STATE_DRIVE;
unsigned long stateStartTime = 0;

void setup() {
  Serial.begin(9600);
  delay(200); // one-time startup delay is fine, this only runs once
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found! Check wiring/address."));
    while (true);
  }

  pinMode(PIN_M11, OUTPUT);
  pinMode(PIN_M12, OUTPUT);
  pinMode(PIN_MOTOR1_PWM, OUTPUT);
  pinMode(PIN_M21, OUTPUT);
  pinMode(PIN_M22, OUTPUT);
  pinMode(PIN_MOTOR2_PWM, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  stopMotors();
}

void loop() {
  SensorReadings readings = readAllSensors();
  printDebug(readings);
  updateStateMachine(readings);
  updateDisplay(readings);
}

// ---------- Non-blocking state machine ----------
// Instead of delay(5000) freezing everything, we just remember WHEN
// each phase started and check "has enough time passed yet?" on
// every loop cycle — the loop never stops running.
void updateStateMachine(const SensorReadings& r) {
  unsigned long elapsed = millis() - stateStartTime;

  switch (currentState) {

    case STATE_DRIVE:
      if (r.obstacleAny) {
        stopMotors();
        lastCommand = "STOP";
        changeState(STATE_STOPPED);
      } else {
        moveForward();
        lastCommand = "FORWARD";
      }
      break;

    case STATE_STOPPED:
      if (elapsed >= PAUSE_TIME_MS) {
        moveBackward();
        lastCommand = "BACKWARD";
        changeState(STATE_REVERSING);
      }
      break;

    case STATE_REVERSING:
      if (elapsed >= BACK_TIME_MS) {
        stopMotors();
        changeState(STATE_TURNING); // GAP_TIME_MS handled at start of TURNING
      }
      break;

    case STATE_TURNING:
      if (elapsed < GAP_TIME_MS) {
        // brief pause before the turn starts (motors already stopped)
      } else if (elapsed < GAP_TIME_MS + TURN_TIME_MS) {
        turnRight();
        lastCommand = "TURN";
      } else {
        stopMotors();
        changeState(STATE_DRIVE); // re-check sensors fresh next cycle
      }
      break;
  }
}

void changeState(RobotState newState) {
  currentState = newState;
  stateStartTime = millis();
}

// ================= Sensors =================
SensorReadings readAllSensors() {
  SensorReadings r;
  r.distanceCM = getMedianDistanceCM();
  r.ir1 = analogRead(PIN_IR1);
  r.ir2 = analogRead(PIN_IR2);

  r.obstacleUltrasonic = (r.distanceCM > 0 && r.distanceCM < OBSTACLE_DISTANCE_CM);
  r.obstacleIR1 = isIRObstacle(r.ir1);
  r.obstacleIR2 = isIRObstacle(r.ir2);
  // Only treat a NEW obstacle as relevant while actively driving —
  // once we're already in the avoidance sequence, sensors are still
  // read (for the display/debug) but don't re-trigger the sequence.
  r.obstacleAny = (currentState == STATE_DRIVE) &&
                  (r.obstacleUltrasonic || r.obstacleIR1 || r.obstacleIR2);

  return r;
}

bool isIRObstacle(int rawValue) {
  return IR_ACTIVE_LOW ? (rawValue < IR_THRESHOLD) : (rawValue > IR_THRESHOLD);
}

long getMedianDistanceCM() {
  long readings[NUM_SAMPLES];
  int count = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    long d = getSingleDistanceCM();
    if (d > 0) readings[count++] = d;
    delay(PING_GAP_MS); // small, unavoidable gap for the sensor itself
  }

  if (count == 0) return -1;

  for (int i = 1; i < count; i++) {
    long key = readings[i];
    int j = i - 1;
    while (j >= 0 && readings[j] > key) {
      readings[j + 1] = readings[j];
      j--;
    }
    readings[j + 1] = key;
  }

  return readings[count / 2];
}

long getSingleDistanceCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  return (duration == 0) ? -1 : (duration * 0.034 / 2);
}

// ================= Motor control =================
void setMotorSpeed(int speed1, int speed2) {
  analogWrite(PIN_MOTOR1_PWM, speed1);
  analogWrite(PIN_MOTOR2_PWM, speed2);
}

void moveForward() {
  digitalWrite(PIN_M11, HIGH); digitalWrite(PIN_M12, LOW);
  digitalWrite(PIN_M21, HIGH); digitalWrite(PIN_M22, LOW);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(PIN_M11, LOW); digitalWrite(PIN_M12, HIGH);
  digitalWrite(PIN_M21, LOW); digitalWrite(PIN_M22, HIGH);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void turnRight() {
  digitalWrite(PIN_M11, HIGH); digitalWrite(PIN_M12, LOW);
  digitalWrite(PIN_M21, LOW);  digitalWrite(PIN_M22, HIGH);
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
}

void stopMotors() {
  digitalWrite(PIN_M11, LOW); digitalWrite(PIN_M12, LOW);
  digitalWrite(PIN_M21, LOW); digitalWrite(PIN_M22, LOW);
  setMotorSpeed(0, 0);
}

// ================= Debug + Display =================
void printDebug(const SensorReadings& r) {
  Serial.print(F("US: "));
  Serial.print(r.distanceCM);
  Serial.print(F("cm | IR1: "));
  Serial.print(r.ir1);
  Serial.print(F(" | IR2: "));
  Serial.print(r.ir2);
  Serial.print(F(" | State: "));
  Serial.println(lastCommand);
}

void updateDisplay(const SensorReadings& r) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Reduce Delays"));

  display.setCursor(0, 14);
  display.print(F("US: "));
  if (r.distanceCM < 0) display.print(F("---"));
  else { display.print(r.distanceCM); display.print(F("cm")); }

  display.setCursor(0, 26);
  display.print(F("IR1:"));
  display.print(r.ir1);
  display.print(F(" IR2:"));
  display.println(r.ir2);

  display.setTextSize(2);
  display.setCursor(0, 44);
  display.println(lastCommand);

  display.display();
}