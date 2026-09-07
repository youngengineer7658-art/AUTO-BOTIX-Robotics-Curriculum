#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define NUM_LEDS  8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(80);
  strip.show();
}

void loop() {
  rainbowCycle(5);
}

void rainbowCycle(uint8_t wait) {

  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {

    for (int i = 0; i < NUM_LEDS; i++) {
      int pixelHue = firstPixelHue + (i * 65536L / NUM_LEDS);
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }

    strip.show();

    Serial.print("Hue: ");
    Serial.println(firstPixelHue);

    delay(wait);
  }
}
