/*
=========================================
Practical-52 : Auto Stop on Obstacle
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

int angle = 0;
bool forward = true;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  Serial.println("===== AUTO STOP ON OBSTACLE =====");
}

int getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) return 999;   // No object = treat as far away

  return duration * 0.034 / 2;
}

void loop()
{
  int distance = getDistance();

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.print(" cm   ");

  if (distance <= 10)
  {
    Serial.println("Obstacle Detected - Servo STOPPED");
    delay(100);
    return;
  }

  Serial.println("Path Clear - Servo Moving");

  myServo.write(angle);

  if (forward)
  {
    angle++;

    if (angle >= 180)
      forward = false;
  }
  else
  {
    angle--;

    if (angle <= 0)
      forward = true;
  }

  delay(15);
}