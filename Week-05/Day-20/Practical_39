#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3
#define BUZZER 12

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const float DETECT_RANGE = 40.0;

float getDistance() {

  long total = 0;
  int validReadings = 0;

  for (int i = 0; i < 5; i++) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    long reading = pulseIn(ECHO, HIGH, 30000);

    if (reading > 0) {
      total += reading;
      validReadings++;
    }

    delay(5);
  }

  if (validReadings == 0)
    return 999;

  return (total / (float)validReadings) / 58.0;
}

void setColor(byte r, byte g, byte b) {

  strip.clear();

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
  Serial.println("===== AUTO DETECT OBSTACLE STARTED =====");
}

void loop() {

  float distance = getDistance();

  // ---------- No Obstacle ----------
  if (distance > DETECT_RANGE) {

    strip.clear();
    strip.show();

    digitalWrite(BUZZER, LOW);

    Serial.println("No Obstacle Detected - LEDs OFF");

    delay(150);
    return;
  }

  // ---------- Obstacle Detected ----------
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.print(" cm   ");

  if (distance > 30) {
    setColor(0, 255, 0);      // Green
    Serial.println("SAFE");

    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(450);                // Slow beep
  }
  else if (distance > 20) {
    setColor(0, 0, 255);      // Blue
    Serial.println("NORMAL");

    digitalWrite(BUZZER, HIGH);
    delay(60);
    digitalWrite(BUZZER, LOW);
    delay(300);                // Medium-slow beep
  }
  else if (distance > 10) {
    setColor(255, 255, 0);    // Yellow
    Serial.println("WARNING");

    digitalWrite(BUZZER, HIGH);
    delay(80);
    digitalWrite(BUZZER, LOW);
    delay(120);                // Medium-fast beep
  }
  else {
    setColor(255, 0, 0);      // Red
    Serial.println("DANGER");

    digitalWrite(BUZZER, HIGH);   // Continuous beep
  }
}
