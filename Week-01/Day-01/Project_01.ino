#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();

  // Full Brightness
  pixels.setBrightness(255);

  // Sabhi LEDs White
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }

  pixels.show();
}

void loop() {
}
