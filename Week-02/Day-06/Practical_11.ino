#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUM_LEDS 8   // Physical strip mein 8 LEDs hain

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void showColor(String name, uint32_t color) {
  strip.clear();                      // pehle sab LEDs off karo
  strip.setPixelColor(0, color);       // sirf LED #0 ko color do
  strip.show();

  Serial.print("Color: ");
  Serial.println(name);

  delay(1000);
}

void setup()
{
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();
}

void loop()
{
  showColor("RED",    strip.Color(255, 0, 0));
  showColor("GREEN",  strip.Color(0, 255, 0));
  showColor("BLUE",   strip.Color(0, 0, 255));
  showColor("YELLOW", strip.Color(255, 255, 0));
  showColor("PURPLE", strip.Color(255, 0, 255));
  showColor("CYAN",   strip.Color(0, 255, 255));
  showColor("WHITE",  strip.Color(255, 255, 255));
}
