/*
==========================================
Project-25 : Mini Crane
Servo -> D10
Potentiometer -> A7
==========================================
*/

#include <Servo.h>

Servo myServo;

#define POT_PIN A7

int potValue;
int angle;
int lastAngle = -1;

void setup()
{
  Serial.begin(9600);

  myServo.attach(10);

  Serial.println("===== MINI CRANE =====");
}

void loop()
{
  potValue = analogRead(POT_PIN);

  angle = map(potValue, 0, 1023, 0, 180);

  myServo.write(angle);

  if (angle != lastAngle) {
    Serial.print("Pot = ");
    Serial.print(potValue);

    Serial.print("   Angle = ");
    Serial.println(angle);

    lastAngle = angle;
  }

  delay(20);
}
