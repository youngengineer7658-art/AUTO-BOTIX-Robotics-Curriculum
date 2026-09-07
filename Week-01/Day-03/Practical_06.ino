#include <Adafruit_NeoPixel.h>

#define LED_PIN     11
#define NUMPIXELS   8
#define BUTTON_PIN  A6

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Ranges (Aapke board ke mutabiq)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

enum Button { BTN_NONE, BTN_SELECT, BTN_UP, BTN_DOWN, BTN_BACK };

bool ledState = false;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

Button readKeypad() {
  int val = analogRead(BUTTON_PIN);
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) return BTN_SELECT;
  if (val >= BTN_UP_MIN     && val <= BTN_UP_MAX)     return BTN_UP;
  if (val >= BTN_DOWN_MIN   && val <= BTN_DOWN_MAX)   return BTN_DOWN;
  if (val >= BTN_BACK_MIN   && val <= BTN_BACK_MAX)   return BTN_BACK;
  return BTN_NONE;
}

void setup() {
  Serial.begin(9600);
  pixels.begin();
  pixels.setBrightness(80);
  pixels.clear();
  pixels.show();
  Serial.println(F("Button Controlled NeoPixel Initialized"));
}

void loop() {
  Button currentBtn = readKeypad();
  bool buttonPressed = (currentBtn != BTN_NONE);

  // Edge detection with non-blocking debounce
  if (buttonPressed && !lastButtonState && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // SELECT button dabane par LED toggle hogi
    if (currentBtn == BTN_SELECT) {
      ledState = !ledState;

      Serial.print(F("Button: SELECT | Raw ADC: "));
      Serial.print(analogRead(BUTTON_PIN));
      Serial.print(F(" | LED State: "));
      Serial.println(ledState ? F("ON (RED)") : F("OFF"));

      if (ledState) {
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // RED
        }
      } else {
        pixels.clear();
      }
      pixels.show();
    }
  }

  lastButtonState = buttonPressed;
}
