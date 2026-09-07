#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define NUM_LEDS  8
#define POT_PIN   A7

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  strip.begin();
  strip.setBrightness(0);
  strip.show();
}

void loop()
{
  // Potentiometer Read
  int potValue = analogRead(POT_PIN);

  // Convert 0-1023 to 0-255
  int brightness = map(potValue, 0, 1023, 0, 255);

  strip.setBrightness(brightness);

  // All LEDs White
  for(int i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(i, strip.Color(255,255,255));
  }

  strip.show();

  delay(10);
}
