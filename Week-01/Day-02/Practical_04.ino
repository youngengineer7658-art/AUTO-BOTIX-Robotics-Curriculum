#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUM_LEDS 8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  strip.begin();
  strip.show();
}

void loop()
{
  // Pattern 1
  strip.clear();
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();
  delay(300);

  // Pattern 2
  strip.clear();
  strip.setPixelColor(2, strip.Color(0, 0, 255));
  strip.setPixelColor(3, strip.Color(255, 255, 0));
  strip.show();
  delay(300);

  // Pattern 3
  strip.clear();
  strip.setPixelColor(4, strip.Color(0, 255, 255));
  strip.setPixelColor(5, strip.Color(255, 0, 255));
  strip.show();
  delay(300);

  // Pattern 4
  strip.clear();
  strip.setPixelColor(6, strip.Color(255, 255, 255));
  strip.setPixelColor(7, strip.Color(255, 100, 0));
  strip.show();
  delay(300);
}
