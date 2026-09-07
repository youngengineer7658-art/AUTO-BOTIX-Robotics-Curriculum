#include <Adafruit_NeoPixel.h>

#define LED_PIN     11
#define NUM_LEDS    8
#define BUTTON_PIN  A6

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Ranges (Aapke Board ka Exact Data)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();
  Serial.println(F("Button Color Switcher Initialized"));
}

void loop() {
  int val = analogRead(BUTTON_PIN);

  // 1. SELECT (931 - 932) -> RED
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) {
    Serial.print(F("Raw: "));
    Serial.print(val);
    Serial.println(F(" | SELECT -> RED"));

    setColor(255, 0, 0);

    while (analogRead(BUTTON_PIN) >= BTN_SELECT_MIN && analogRead(BUTTON_PIN) <= BTN_SELECT_MAX);
    delay(150);
  }

  // 2. UP (853 - 855) -> GREEN
  else if (val >= BTN_UP_MIN && val <= BTN_UP_MAX) {
    Serial.print(F("Raw: "));
    Serial.print(val);
    Serial.println(F(" | UP -> GREEN"));

    setColor(0, 255, 0);

    while (analogRead(BUTTON_PIN) >= BTN_UP_MIN && analogRead(BUTTON_PIN) <= BTN_UP_MAX);
    delay(150);
  }

  // 3. DOWN (789 - 790) -> BLUE
  else if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX) {
    Serial.print(F("Raw: "));
    Serial.print(val);
    Serial.println(F(" | DOWN -> BLUE"));

    setColor(0, 0, 255);

    while (analogRead(BUTTON_PIN) >= BTN_DOWN_MIN && analogRead(BUTTON_PIN) <= BTN_DOWN_MAX);
    delay(150);
  }

  // 4. BACK (733 - 734) -> WHITE
  else if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) {
    Serial.print(F("Raw: "));
    Serial.print(val);
    Serial.println(F(" | BACK -> WHITE"));

    setColor(255, 255, 255);

    while (analogRead(BUTTON_PIN) >= BTN_BACK_MIN && analogRead(BUTTON_PIN) <= BTN_BACK_MAX);
    delay(150);
  }
}
