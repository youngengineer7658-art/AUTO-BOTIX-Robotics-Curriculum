/*
==========================================
Project : Robot Dashboard
Autobotix Robotics Board (Verified Pins)
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define M11 2
#define M12 4
#define MFN 6       // Motor 1 Enable / PWM
#define M2EN 9      // Motor 2 Enable / PWM
#define M21 7
#define M22 8

#define TRIG_PIN 5
#define ECHO_PIN A3
#define BUTTON_PIN A6
#define POT_PIN A7

#define RGB_PIN 11
#define NUMPIXELS 8
Adafruit_NeoPixel rgb(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

#define BUZZER_PIN 12

bool motorRunning = false;
int speedValue = 150;
int lastButton = 0;

//---------------------------------
int getButton()
{
  int value = analogRead(BUTTON_PIN);

  // UP (853 - 855)
  if (value >= 845 && value <= 865) return 1;

  // DOWN (789 - 790)
  if (value >= 780 && value <= 800) return 2;

  // BACK (733 - 734)
  if (value >= 725 && value <= 745) return 3;

  // SELECT (931 - 932)
  if (value >= 920 && value <= 945) return 4;

  return 0;
}

//---------------------------------
int getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 15000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

//---------------------------------
void setRGB(byte r, byte g, byte b)
{
  for (int i = 0; i < NUMPIXELS; i++)
    rgb.setPixelColor(i, rgb.Color(r, g, b));
  rgb.show();
}

//---------------------------------
void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 0);
  analogWrite(M2EN, 0);
}

//---------------------------------
void driveForward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
  analogWrite(MFN, speedValue);
  analogWrite(M2EN, speedValue);
}

//---------------------------------
void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);
  pinMode(M2EN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  stopMotor();

  rgb.begin();
  rgb.clear();
  rgb.show();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while (1);
  }

  display.setTextColor(SSD1306_WHITE);

  Serial.println("===== ROBOT DASHBOARD STARTED =====");
}

//---------------------------------
void loop()
{
  int distance = getDistance();
  int pot = analogRead(POT_PIN);
  speedValue = map(pot, 0, 1023, 60, 255);
  int btn = getButton();

  if (btn != 0 && lastButton == 0)
  {
    if (btn == 4) {
      motorRunning = !motorRunning;
      Serial.println("SELECT pressed -> toggle");
    }
    if (btn == 3) {
      motorRunning = false;
      Serial.println("BACK pressed -> force stop");
    }
  }

  lastButton = btn;

  bool obstacle = (distance != -1 && distance <= 15);

  if (obstacle) {
    stopMotor();
    setRGB(255, 0, 0);
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else if (motorRunning) {
    driveForward();
    setRGB(0, 255, 0);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else {
    stopMotor();
    setRGB(0, 0, 255);
    digitalWrite(BUZZER_PIN, LOW);
  }

  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(28, 3);
  display.println("ROBOT DASHBOARD");
  display.drawLine(5, 13, 122, 13, SSD1306_WHITE);

  display.setCursor(5, 20);
  display.print("DIST  : ");
  if (distance == -1) display.print("--");
  else display.print(distance);
  display.print(" cm");

  display.setCursor(5, 32);
  display.print("SPEED : ");
  display.print(speedValue);

  display.setCursor(5, 44);
  display.print("MOTOR : ");
  display.print(motorRunning ? "RUNNING" : "STOPPED");

  display.setCursor(5, 56);
  if (obstacle)
    display.print("!! OBSTACLE !!");
  else
    display.print("PATH CLEAR");

  display.display();

  delay(20);
}
