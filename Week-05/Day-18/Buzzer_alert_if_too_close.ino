#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3
#define BUZZER 12

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const float SAFE_DISTANCE = 15.0;   // isse kam ho to "too close"

float getDistance() {

  long total = 0;

  for (int i = 0; i < 5; i++) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    total += pulseIn(ECHO, HIGH, 30000);
    delay(10);
  }

  return (total / 5.0) / 58.0;
}

void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < LED_COUNT; i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== BUZZER ALERT IF TOO CLOSE STARTED =====");
}

void loop() {

  float distance = getDistance();

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.print(" cm   ");

  if (distance <= SAFE_DISTANCE) {

    Serial.println("TOO CLOSE - ALERT!");

    setColor(255, 0, 0);        // Red

    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
  }
  else {

    Serial.println("SAFE");

    setColor(0, 255, 0);        // Green
    digitalWrite(BUZZER, LOW);
    delay(150);
  }
}