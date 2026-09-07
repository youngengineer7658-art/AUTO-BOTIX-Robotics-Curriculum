#include <Adafruit_NeoPixel.h>

#define LED_PIN 11
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Sab LEDs ON
void allOn() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red
  }
  pixels.show();
}

// Sab LEDs OFF
void allOff() {
  pixels.clear();
  pixels.show();
}

// Short Blink (S)
void shortBlink() {
  allOn();
  delay(200);
  allOff();
  delay(200);
}

// Long Blink (O)
void longBlink() {
  allOn();
  delay(600);
  allOff();
  delay(200);
}

void setup() {
  pixels.begin();
  pixels.setBrightness(80);
  allOff();
}

void loop() {

  // S (...)
  shortBlink();
  shortBlink();
  shortBlink();

  delay(400);

  // O (---)
  longBlink();
  longBlink();
  longBlink();

  delay(400);

  // S (...)
  shortBlink();
  shortBlink();
  shortBlink();

  // SOS repeat hone se pehle pause
  delay(1500);
}
