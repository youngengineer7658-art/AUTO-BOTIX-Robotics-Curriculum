/*
=========================================
Practical-51 : Servo Move Based on Distance
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

long duration;
int distance;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  myServo.write(0);

  Serial.println("===== Servo Move Based on Distance =====");
}

void loop()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    Serial.println("Out of Range");
    myServo.write(0);
    delay(300);
    return;
  }

  distance = duration * 0.034 / 2;

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= 30)
  {
    myServo.write(180);
    Serial.println("Servo = 180");
  }
  else
  {
    myServo.write(0);
    Serial.println("Servo = 0");
  }

  delay(300);
}
