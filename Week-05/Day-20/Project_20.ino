#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3
#define BUZZER 12

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long duration;
float distance;

// -------- Read Distance --------
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

// -------- Set RGB Color --------
void setColor(byte r, byte g, byte b) {

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }

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

  Serial.println("Obstacle Detector Alarm");
}

void loop() {

  distance = getDistance();

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");

  // -------- SAFE --------
  if (distance > 30) {

    setColor(0, 255, 0);      // Green
    digitalWrite(BUZZER, LOW);

    Serial.println("SAFE");

  }

  // -------- WARNING --------
  else if (distance > 15) {

    setColor(255, 255, 0);    // Yellow

    Serial.println("WARNING");

    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(300);

  }

  // -------- DANGER --------
  else {

    setColor(255, 0, 0);      // Red

    Serial.println("DANGER");

    digitalWrite(BUZZER, HIGH);
    delay(80);
    digitalWrite(BUZZER, LOW);
    delay(80);

  }
}
