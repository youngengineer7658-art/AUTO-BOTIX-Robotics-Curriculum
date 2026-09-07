/*
==========================================
Project-34 : Radar System (Clean Serial)
==========================================
*/

#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SERVO_PIN 10
#define TRIG 5
#define ECHO A3

Servo radarServo;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//--------------------------------
int getDistance()
//--------------------------------
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0)
    return 400;

  return duration * 0.034 / 2;
}

//--------------------------------
void showOLED(int angle, int distance)
//--------------------------------
{
  display.clearDisplay();

  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(48, 3);
  display.println("RADAR");

  display.drawLine(5, 14, 122, 14, SSD1306_WHITE);

  display.setCursor(8, 20);
  display.print("ANGLE  : ");
  display.print(angle);
  display.print((char)247);

  display.setCursor(8, 34);
  display.print("DIST   : ");
  if (distance == 400) {
    display.print("Out Range");
  } else {
    display.print(distance);
    display.print(" cm");
  }

  display.setCursor(8, 48);
  if (distance <= 20)
  {
    display.print("STATUS : DETECTED!");
  }
  else
  {
    display.print("STATUS : CLEAR");
  }

  display.display();
}

//--------------------------------
void printSerial(int angle, int distance)
//--------------------------------
{
  Serial.print("Angle: ");
  if (angle < 10) Serial.print("  ");
  else if (angle < 100) Serial.print(" ");
  Serial.print(angle);
  Serial.print("° \t| Distance: ");

  if (distance == 400) {
    Serial.print("Out of Range");
  } else {
    Serial.print(distance);
    Serial.print(" cm");
  }

  Serial.print("\t| Status: ");
  if (distance <= 20) {
    Serial.println("ALERT! Target Detected");
  } else {
    Serial.println("Clear");
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  radarServo.attach(SERVO_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (1);
  }

  display.setTextColor(SSD1306_WHITE);

  Serial.println("===========================================");
  Serial.println("           RADAR SYSTEM STARTED            ");
  Serial.println("===========================================");
}

void loop()
{
  // Left to Right
  for (int angle = 0; angle <= 180; angle += 5)
  {
    radarServo.write(angle);
    delay(80);

    int distance = getDistance();

    printSerial(angle, distance);
    showOLED(angle, distance);
  }

  // Right to Left
  for (int angle = 180; angle >= 0; angle -= 5)
  {
    radarServo.write(angle);
    delay(80);

    int distance = getDistance();

    printSerial(angle, distance);
    showOLED(angle, distance);
  }
}
