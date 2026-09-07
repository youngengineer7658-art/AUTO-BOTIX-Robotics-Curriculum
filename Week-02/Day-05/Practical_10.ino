#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUM_LEDS 8
#define POT_PIN A7

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int lastBrightness = -1;
const int THRESHOLD = 3;

void setup()
{
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();
}

void loop()
{
  // Potentiometer Read
  int potValue = analogRead(POT_PIN);

  // 0-1023 ko 0-255 me convert karo
  int brightness = map(potValue, 0, 1023, 0, 255);

  strip.setBrightness(brightness);

  // Sab LEDs White
  for (int i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }

  strip.show();

  // Sirf tab print karo jab brightness meaningfully change ho
  if (abs(brightness - lastBrightness) > THRESHOLD)
  {
    Serial.print("Pot Raw: ");
    Serial.print(potValue);
    Serial.print(" | Brightness: ");
    Serial.println(brightness);

    lastBrightness = brightness;
  }

  delay(10);
}
