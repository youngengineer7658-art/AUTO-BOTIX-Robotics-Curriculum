#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long duration;
float distance;

// Ruler range: 0 cm se 40 cm tak, 8 LEDs mein divide kiya
const float MAX_DISTANCE = 40.0;

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  strip.begin();
  strip.clear();
  strip.show();

  Serial.begin(9600);
  Serial.println("===== DIGITAL RULER STARTED =====");
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
  }

  else {

    distance = duration / 58.0;

    // Distance ko 0-8 LEDs ke range mein map karo
    int litLEDs = map(distance, 0, MAX_DISTANCE, LED_COUNT, 0);

    // Safety limit (agar distance MAX se zyada ho ya negative aaye)
    litLEDs = constrain(litLEDs, 0, LED_COUNT);

    strip.clear();

    for (int i = 0; i < litLEDs; i++) {
      strip.setPixelColor(i, strip.Color(255, 160, 60));   // Warm Yellow
    }

    strip.show();

    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.print(" cm   |   LEDs Lit = ");
    Serial.println(litLEDs);
  }

  delay(150);
}