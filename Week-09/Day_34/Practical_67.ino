/*
==========================================
Practical-67 : Servo Scan Environment (Perfected)
Servo Pin -> D10
Trig Pin  -> D5
Echo Pin  -> A3
==========================================
*/

#include <Servo.h>

#define SERVO_PIN 10
#define TRIG 5
#define ECHO A3

Servo radarServo;

//----------------------------
// Accurate Distance Function with Filter
//----------------------------
int getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Read pulse with 25ms timeout (~400cm max)
  long duration = pulseIn(ECHO, HIGH, 25000);

  if (duration == 0) {
    return 400; // Out of range
  }

  int dist = duration * 0.034 / 2;
  return dist;
}

// Filter to remove fake jumps/errors
int getFilteredDistance()
{
  int d1 = getDistance();
  delay(10);
  int d2 = getDistance();
  
  // Return minimum valid reading
  return min(d1, d2);
}

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  radarServo.attach(SERVO_PIN);

  // Smooth start
  radarServo.write(90);
  delay(1000);
}

void loop()
{
  // Forward Sweep: 0° → 180°
  for (int angle = 0; angle <= 180; angle += 10) // 10 degree step for stability
  {
    radarServo.write(angle);
    delay(100); // Give servo time to settle

    int distance = getFilteredDistance();

    Serial.print("Angle : ");
    Serial.print(angle);
    Serial.print("° \t Distance : ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Reverse Sweep: 180° → 0°
  for (int angle = 180; angle >= 0; angle -= 10)
  {
    radarServo.write(angle);
    delay(100);

    int distance = getFilteredDistance();

    Serial.print("Angle : ");
    Serial.print(angle);
    Serial.print("° \t Distance : ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}
