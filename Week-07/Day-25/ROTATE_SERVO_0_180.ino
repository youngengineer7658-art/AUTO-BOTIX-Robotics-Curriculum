/*
==========================================
Practical-49 : Rotate Servo 0°–180°
Servo Pin : D10
==========================================
*/

#include <Servo.h>

Servo myServo;

void setup()
{
  Serial.begin(9600);

  myServo.attach(10);

  Serial.println("===== Servo 0°-180° Sweep Test =====");
}

void loop()
{
  // 0° se 180° tak dheere-dheere jao
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle);

    Serial.print("Servo Angle = ");
    Serial.println(angle);

    delay(15);
  }

  delay(500);

  // 180° se wapas 0° tak dheere-dheere jao
  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle);

    Serial.print("Servo Angle = ");
    Serial.println(angle);

    delay(15);
  }

  delay(500);
}