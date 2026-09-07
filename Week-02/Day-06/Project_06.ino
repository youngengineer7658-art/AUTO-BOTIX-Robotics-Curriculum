#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUM_LEDS 8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(80);
  strip.show();

  Serial.println("===== DISCO LIGHT STARTED =====");
}

void loop()
{
  Serial.println("Effect: Rainbow");
  rainbow(3);

  Serial.println("Effect: Solid RED");
  solidColor(255, 0, 0, 500);

  Serial.println("Effect: Solid GREEN");
  solidColor(0, 255, 0, 500);

  Serial.println("Effect: Solid BLUE");
  solidColor(0, 0, 255, 500);

  Serial.println("Effect: Police Lights");
  police();

  Serial.println("Effect: Strobe");
  strobe(10, 80, 80);

  Serial.println("Effect: Disco Sparkle");
  discoSparkle(15);

  Serial.println("Effect: Running Light");
  running();

  Serial.println("----- Loop Complete, Restarting -----");
}

// ---------------- Solid Color ----------------
void solidColor(byte r, byte g, byte b, int t)
{
  for (int i = 0; i < NUM_LEDS; i++)
    strip.setPixelColor(i, strip.Color(r, g, b));

  strip.show();
  delay(t);
}

// ---------------- Running Light ----------------
void running()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    strip.clear();
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(120);
  }
}

// ---------------- Police Lights ----------------
void police()
{
  for (int k = 0; k < 6; k++)
  {
    strip.clear();
    for (int i = 0; i < 4; i++)
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    for (int i = 4; i < 8; i++)
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    strip.show();
    delay(120);

    strip.clear();
    strip.show();
    delay(80);

    strip.clear();
    for (int i = 0; i < 4; i++)
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    for (int i = 4; i < 8; i++)
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(120);

    strip.clear();
    strip.show();
    delay(80);
  }
}

// ---------------- Rainbow ----------------
void rainbow(int wait)
{
  for (long hue = 0; hue < 65536; hue += 256)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i,
        strip.gamma32(
          strip.ColorHSV(hue + i * 65536L / NUM_LEDS)));
    }
    strip.show();
    delay(wait);
  }
}

// ---------------- Strobe ----------------
void strobe(int times, int onTime, int offTime)
{
  for (int i = 0; i < times; i++)
  {
    for (int j = 0; j < NUM_LEDS; j++)
      strip.setPixelColor(j, strip.Color(255, 255, 255));
    strip.show();
    delay(onTime);

    strip.clear();
    strip.show();
    delay(offTime);
  }
}

// ---------------- Disco Sparkle ----------------
void discoSparkle(int cycles)
{
  for (int c = 0; c < cycles; c++)
  {
    strip.clear();
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(random(0, 255), random(0, 255), random(0, 255)));
    }
    strip.show();
    delay(100);
  }
}
