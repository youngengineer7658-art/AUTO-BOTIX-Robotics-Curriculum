/*
==========================================
Project-30 : Ultrasonic Distance Meter
OLED SSD1306 128x64
TRIG -> D5
ECHO -> A3
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TRIG_PIN 5
#define ECHO_PIN A3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

long duration;
int distance;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  // Trigger Pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Read Echo
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    distance = -1;
  else
    distance = duration * 0.034 / 2;

  // Serial Monitor
  Serial.print("Distance : ");

  if (distance == -1)
    Serial.println("Out of Range");
  else
  {
    Serial.print(distance);
    Serial.println(" cm");
  }

  // OLED
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(8, 0);
  display.println("DISTANCE");

  display.drawLine(0, 20, 127, 20, SSD1306_WHITE);

  if (distance == -1)
  {
    display.setTextSize(1);
    display.setCursor(10, 40);
    display.println("OUT OF RANGE");
  }
  else
  {
    display.setTextSize(3);
    display.setCursor(10, 30);
    display.print(distance);
    display.setTextSize(1);
    display.print(" cm");
  }

  display.display();

  delay(200);
}