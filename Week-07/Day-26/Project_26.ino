/*
=========================================
Project-26 : Smart Obstacle Avoider Arm
Servo  -> D10
TRIG   -> D5
ECHO   -> A3
=========================================
*/

#include <Servo.h>

#define TRIG_PIN 5
#define ECHO_PIN A3
#define SERVO_PIN 10

Servo myServo;

bool obstacleHandled = false;

//==========================
int getDistance()
//==========================
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return 999;

  return duration * 0.034 / 2;
}

//==========================
void setup()
//==========================
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  myServo.write(0);

  Serial.println("===== Smart Obstacle Arm =====");
}

//==========================
void loop()
//==========================
{
  int distance = getDistance();

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");

  // First time obstacle detected
  if (distance <= 15 && !obstacleHandled)
  {
    Serial.println("Obstacle Detected");

    myServo.write(90);
    delay(1000);

    myServo.write(180);
    delay(1000);

    obstacleHandled = true;

    Serial.println("Waiting for obstacle to move...");
  }

  // Obstacle removed
  if (distance > 15 && obstacleHandled)
  {
    Serial.println("Obstacle Removed");

    myServo.write(0);

    obstacleHandled = false;
  }

  delay(200);
}
