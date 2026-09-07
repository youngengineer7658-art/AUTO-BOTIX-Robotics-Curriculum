/*
====================================================
 Practical: Print Sensor Values on OLED
 Sensors: Potentiometer (A7) + Ultrasonic (Trig D5, Echo A3)
 Display: SSD1306 128x64 I2C
====================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Correct Pin Definitions
#define POT_PIN   A7
#define TRIG_PIN  5
#define ECHO_PIN  A3

long duration;
int distanceCm;
int potValue;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("OLED Failed!"));
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  // 1. Read Potentiometer Value
  potValue = analogRead(POT_PIN);

  // 2. Read Ultrasonic Distance
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout

  if (duration == 0) {
    distanceCm = 0;
  } else {
    distanceCm = duration * 0.034 / 2;
  }

  // 3. Serial Monitor Output
  Serial.print("POT: ");
  Serial.print(potValue);
  Serial.print(" | Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // 4. OLED Display Output
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("--- SENSOR VALUES ---");

  // POT Reading
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print("POT :");
  display.print(potValue);

  // Ultrasonic Reading
  display.setCursor(0, 42);
  display.print("DIST:");
  display.print(distanceCm);
  display.print("cm");

  display.display();

  delay(200);
}