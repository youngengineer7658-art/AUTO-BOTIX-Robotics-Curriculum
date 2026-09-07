/*
==========================================
Practical-68 : Show Distance on OLED (Smooth Real-time)
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

// Single Raw Reading
int readRawDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 20000); // 20ms timeout

  if (duration == 0) return 400;

  int cm = duration * 0.034 / 2;
  if (cm < 2 || cm > 400) return 400;
  
  return cm;
}

// Real-Time Median Filter (Locks Nahi Hoga)
int getCleanDistance()
{
  int a = readRawDistance();
  delay(10);
  int b = readRawDistance();

  // Agar dono me se ek valid distance hai, to wo return karo
  if (a != 400 && b != 400) return (a + b) / 2;
  if (a != 400) return a;
  if (b != 400) return b;
  
  return 400;
}

void showOLED(int angle, int distance)
{
  display.clearDisplay();

  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 3);
  display.println("RADAR SYSTEM");

  display.drawLine(5, 14, 122, 14, SSD1306_WHITE);

  display.setCursor(8, 24);
  display.print("ANGLE : ");
  display.print(angle);
  display.print((char)247);

  display.setCursor(8, 42);
  display.print("DIST  : ");
  if (distance == 400) {
    display.print("Out Range");
  } else {
    display.print(distance);
    display.print(" cm");
  }

  display.display();
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
}

void loop()
{
  // 0° → 180°
  for (int angle = 0; angle <= 180; angle += 5)
  {
    radarServo.write(angle);
    delay(100); // Servo ko rukne ka mauka do

    int distance = getCleanDistance();

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("  Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    showOLED(angle, distance);
  }

  // 180° → 0°
  for (int angle = 180; angle >= 0; angle -= 5)
  {
    radarServo.write(angle);
    delay(100);

    int distance = getCleanDistance();

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("  Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    showOLED(angle, distance);
  }
}