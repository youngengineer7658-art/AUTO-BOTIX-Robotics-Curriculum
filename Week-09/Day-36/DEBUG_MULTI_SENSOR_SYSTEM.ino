// ==========================================
// Practical-71 : Debug Multi Sensor System
// ==========================================

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TRIG_PIN 5
#define ECHO_PIN A3

#define SERVO_PIN 10
#define BUZZER_PIN 12
#define LED_PIN 13

#define BUTTON_PIN A6
#define POT_PIN A7

Servo servo1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Calibrated Button Ranges
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

String getButtonName(int val)
{
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) return "SELECT";
  if (val >= BTN_UP_MIN && val <= BTN_UP_MAX)         return "UP";
  if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX)     return "DOWN";
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX)     return "BACK";
  return "NONE";
}

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  servo1.attach(SERVO_PIN);
  servo1.write(90);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while (1);
  }

  display.setTextColor(SSD1306_WHITE);

  Serial.println("=== Practical-71 ===");
}

void loop()
{
  // ---------- Ultrasonic ----------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance;

  if (duration == 0)
    distance = 999;
  else
    distance = duration * 0.0343 / 2.0;

  // ---------- Potentiometer ----------
  int potValue = analogRead(POT_PIN);

  // ---------- Buttons ----------
  int buttonValue = analogRead(BUTTON_PIN);
  String btnName = getButtonName(buttonValue);

  // ---------- Servo ----------
  int angle = map(potValue, 0, 1023, 0, 180);
  servo1.write(angle);

  // ---------- LED + Buzzer ----------
  if (distance <= 20)
  {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ---------- Serial Output ----------
  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.print(" cm");

  Serial.print(" | POT : ");
  Serial.print(potValue);

  Serial.print(" | Button : ");
  Serial.print(buttonValue);
  Serial.print(" (");
  Serial.print(btnName);
  Serial.print(")");

  Serial.print(" | Servo : ");
  Serial.println(angle);

  // ---------- OLED Output ----------
  display.clearDisplay();

  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(15, 3);
  display.println("SENSOR DEBUG");
  display.drawLine(5, 13, 122, 13, SSD1306_WHITE);

  display.setCursor(5, 20);
  display.print("DIST : ");
  if (distance == 999) display.print("--");
  else display.print((int)distance);
  display.print(" cm");

  display.setCursor(5, 32);
  display.print("POT  : ");
  display.print(potValue);

  display.setCursor(5, 44);
  display.print("BTN  : ");
  display.print(buttonValue);
  display.print(" ");
  display.print(btnName);

  display.setCursor(5, 56);
  display.print("SERVO: ");
  display.print(angle);
  display.print((char)247);

  display.display();

  delay(200);
}