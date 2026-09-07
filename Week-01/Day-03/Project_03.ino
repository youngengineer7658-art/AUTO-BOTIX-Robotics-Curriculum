#include <Adafruit_NeoPixel.h>

#define LED_PIN     11
#define NUM_LEDS    8
#define BUTTON_PIN  A6

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Ranges (Aapke Board ki Exact Values)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

enum Button { BTN_NONE, BTN_SELECT, BTN_UP, BTN_DOWN, BTN_BACK };

bool torch = false;
bool rainbow = false;
int brightness = 100;
uint16_t hue = 0;

Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
unsigned long lastRainbowUpdate = 0;

Button readKeypad() {
  int val = analogRead(BUTTON_PIN);
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) return BTN_SELECT;
  if (val >= BTN_UP_MIN     && val <= BTN_UP_MAX)     return BTN_UP;
  if (val >= BTN_DOWN_MIN   && val <= BTN_DOWN_MAX)   return BTN_DOWN;
  if (val >= BTN_BACK_MIN   && val <= BTN_BACK_MAX)   return BTN_BACK;
  return BTN_NONE;
}

void showTorch() {
  strip.setBrightness(brightness);
  if (torch) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255)); // White Light
    }
  } else {
    strip.clear();
  }
  strip.show();
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();
  Serial.println(F("RGB Multi-Mode Controller Initialized"));
}

void loop() {
  Button currentBtn = readKeypad();
  int rawADC = analogRead(BUTTON_PIN);

  // Single press detection with debounce
  if (currentBtn != BTN_NONE && currentBtn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // 1. SELECT Button (931 - 932) -> Torch ON/OFF
    if (currentBtn == BTN_SELECT) {
      torch = !torch;
      rainbow = false;

      Serial.print(F("SELECT Pressed | Raw: "));
      Serial.print(rawADC);
      Serial.print(F(" | Torch: "));
      Serial.println(torch ? F("ON") : F("OFF"));

      showTorch();
    }

    // 2. BACK Button (733 - 734) -> Rainbow Mode Toggle
    else if (currentBtn == BTN_BACK) {
      rainbow = !rainbow;
      torch = false;

      Serial.print(F("BACK Pressed | Raw: "));
      Serial.print(rawADC);
      Serial.print(F(" | Rainbow: "));
      Serial.println(rainbow ? F("ON") : F("OFF"));

      if (!rainbow) {
        strip.clear();
        strip.show();
      }
    }

    // 3. UP Button (853 - 855) -> Increase Brightness
    else if (currentBtn == BTN_UP) {
      brightness += 25;
      if (brightness > 255) brightness = 255;

      Serial.print(F("UP Pressed | Raw: "));
      Serial.print(rawADC);
      Serial.print(F(" | Brightness: "));
      Serial.println(brightness);

      if (torch) showTorch();
    }

    // 4. DOWN Button (789 - 790) -> Decrease Brightness
    else if (currentBtn == BTN_DOWN) {
      brightness -= 25;
      if (brightness < 20) brightness = 20;

      Serial.print(F("DOWN Pressed | Raw: "));
      Serial.print(rawADC);
      Serial.print(F(" | Brightness: "));
      Serial.println(brightness);

      if (torch) showTorch();
    }
  }

  lastButton = currentBtn;

  // Non-blocking Rainbow Animation
  if (rainbow && (millis() - lastRainbowUpdate > 20)) {
    lastRainbowUpdate = millis();
    strip.setBrightness(brightness);

    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue + (i * 65536L / NUM_LEDS))));
    }
    strip.show();
    hue += 256;
  }
}
