#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const float DETECT_RANGE = 40.0;

int lastLeds = -1;

// -------- Read Average Distance --------
float getDistance() {

  long total = 0;

  for (int i = 0; i < 5; i++) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    total += pulseIn(ECHO, HIGH, 30000);

    delay(5);
  }

  return (total / 5.0) / 58.0;
}

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== DISTANCE BAR GRAPH STARTED =====");
}

void loop() {

  float distance = getDistance();

  // ---------- No Object Detected ----------
  if (distance <= 0 || distance > DETECT_RANGE) {

    strip.clear();
    strip.show();

    if (lastLeds != 0) {
      Serial.println("No Object Detected | LEDs OFF");
      lastLeds = 0;
    }

    delay(100);
    return;
  }

  // ---------- Object Detected ----------
  int leds = 0;

  if (distance <= 5)
    leds = 8;
  else if (distance <= 10)
    leds = 7;
  else if (distance <= 15)
    leds = 6;
  else if (distance <= 20)
    leds = 5;
  else if (distance <= 25)
    leds = 4;
  else if (distance <= 30)
    leds = 3;
  else if (distance <= 35)
    leds = 2;
  else
    leds = 1;

  strip.clear();

  for (int i = 0; i < leds; i++) {

    if (i < 3)
      strip.setPixelColor(i, strip.Color(0, 255, 0));      // Green

    else if (i < 6)
      strip.setPixelColor(i, strip.Color(255, 255, 0));    // Yellow

    else
      strip.setPixelColor(i, strip.Color(255, 0, 0));      // Red
  }

  strip.show();

  // Sirf tab print karo jab LED count change ho
  if (leds != lastLeds) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm  |  LEDs ON: ");
    Serial.println(leds);

    lastLeds = leds;
  }

  delay(100);
}
