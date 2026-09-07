// Project : Distance Meter (with RGB LED Display)

#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const float DETECT_RANGE = 40.0;   // isse door = "no object"

long duration;
float distance;
float lastDistance = -1;
const float THRESHOLD = 0.5;

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== DISTANCE METER STARTED =====");
}

void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < LED_COUNT; i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}

void loop() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  distance = (duration == 0) ? 999 : duration / 58.0;

  // ---------- No Object (stable OFF) ----------
  if (distance > DETECT_RANGE) {

    strip.clear();
    strip.show();

    if (lastDistance != -1) {
      Serial.println("No Object - LEDs OFF");
      lastDistance = -1;
    }

    delay(150);
    return;
  }

  // ---------- Object Detected ----------
  if (distance <= 10) {
    setColor(255, 0, 0);        // Red - Very Close
  }
  else if (distance <= 20) {
    setColor(255, 255, 0);      // Yellow - Close
  }
  else if (distance <= 30) {
    setColor(0, 0, 255);        // Blue - Medium
  }
  else {
    setColor(0, 255, 0);        // Green - Far (within range)
  }

  if (abs(distance - lastDistance) > THRESHOLD) {

    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" cm");

    lastDistance = distance;
  }

  delay(150);
}
