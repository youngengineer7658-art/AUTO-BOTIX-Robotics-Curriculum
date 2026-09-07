/*
==========================================
Practical-50 : Smooth Servo Sweep
Servo Pin : D10
==========================================
*/

#include <Servo.h>

Servo myServo;

void setup()
{
  Serial.begin(9600);

  myServo.attach(10);

  Serial.println("===== Servo Sweep Test =====");
}

void loop()
{
  Serial.println("Moving 0° -> 180°");

  for(int angle = 0; angle <= 180; angle++)
  {
    myServo.write(angle);
    delay(15);
  }

  delay(500);

  Serial.println("Moving 180° -> 0°");

  for(int angle = 180; angle >= 0; angle--)
  {
    myServo.write(angle);
    delay(15);
  }

  delay(500);
}
