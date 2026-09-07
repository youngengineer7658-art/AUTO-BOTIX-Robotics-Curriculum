#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long duration;
float distance;

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== DISTANCE BASED LED COLOUR STARTED =====");
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

  if (duration == 0) {

    strip.clear();
    strip.show();

    Serial.println("Out of Range");

    delay(200);
    return;
  }

  distance = duration / 58.0;

  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.print(" cm   ");

  if (distance <= 10) {
    Serial.println("VERY CLOSE - RED");
    setColor(255, 0, 0);        // Red
  }
  else if (distance <= 20) {
    Serial.println("CLOSE - YELLOW");
    setColor(255, 255, 0);      // Yellow
  }
  else if (distance <= 30) {
    Serial.println("MEDIUM - BLUE");
    setColor(0, 0, 255);        // Blue
  }
  else {
    Serial.println("FAR - GREEN");
    setColor(0, 255, 0);        // Green
  }

  delay(150);
}
