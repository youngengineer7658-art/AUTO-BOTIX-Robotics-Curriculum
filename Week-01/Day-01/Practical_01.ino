#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.setBrightness(80);
}

void loop() {

  // Sab LEDs OFF
  pixels.clear();

  // Sirf pehli LED RED ON
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();

  delay(500);

  // Sab LEDs OFF
  pixels.clear();
  pixels.show();

  delay(500);
}
