#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define NUM_LEDS  8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void redLight() {
  strip.clear();
  strip.setPixelColor(0, strip.Color(255, 0, 0));     // LED1 = Red
  strip.show();

  Serial.println("RED - STOP");
}

void greenLight() {
  strip.clear();
  strip.setPixelColor(2, strip.Color(0, 255, 0));     // LED3 = Green
  strip.show();

  Serial.println("GREEN - GO");
}

void yellowLight() {
  strip.clear();
  strip.setPixelColor(1, strip.Color(255, 180, 0));   // LED2 = Yellow
  strip.show();

  Serial.println("YELLOW - READY/CAUTION");
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();

  Serial.println("===== TRAFFIC LIGHT STARTED =====");
}

void loop() {

  // 🔴 RED - Stop
  redLight();
  delay(3000);

  // 🟢 GREEN - Go
  greenLight();
  delay(3000);

  // 🟡 YELLOW - Caution (before switching back to red)
  yellowLight();
  delay(1000);

  // Cycle repeats automatically
}
