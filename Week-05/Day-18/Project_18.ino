#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3
#define BUZZER 12

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long duration;
float distance;

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== PARKING ASSISTANT STARTED =====");
}

void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < LED_COUNT; i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
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

    digitalWrite(BUZZER, LOW);

    Serial.println("Out of Range");

    delay(200);
    return;
  }

  distance = duration / 58.0;

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.print(" cm   ");

  strip.clear();

  if (distance > 30) {

    Serial.println("SAFE");
    setColor(0, 255, 0);        // Green
    digitalWrite(BUZZER, LOW);  // No beep
  }

  else if (distance > 20) {

    Serial.println("CAUTION");
    setColor(0, 0, 255);        // Blue

    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(450);                 // Slow beep
  }

  else if (distance > 10) {

    Serial.println("CLOSE - SLOW DOWN");
    setColor(255, 255, 0);      // Yellow

    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(200);                 // Medium beep
  }

  else {

    Serial.println("STOP! TOO CLOSE");
    setColor(255, 0, 0);        // Red

    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(50);                  // Fast continuous beep
  }

  strip.show();
}
