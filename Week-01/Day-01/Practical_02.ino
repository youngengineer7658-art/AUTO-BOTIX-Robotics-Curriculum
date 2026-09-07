#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.setBrightness(80);
}

void loop() {

  // LED ON
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  delay(100);   // 100ms ON

  // LED OFF
  pixels.clear();
  pixels.show();
  delay(100);   // 100ms OFF
}
