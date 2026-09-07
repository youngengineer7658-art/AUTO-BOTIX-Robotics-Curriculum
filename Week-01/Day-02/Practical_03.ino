#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.setBrightness(80);
}

void loop() {

  // Forward
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
    delay(150);
  }

  // Backward
  for (int i = NUMPIXELS - 2; i > 0; i--) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
    delay(150);
  }
}
