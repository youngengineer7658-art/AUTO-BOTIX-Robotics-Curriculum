#include <Adafruit_NeoPixel.h>

#define LED_PIN    11
#define LED_COUNT  8
#define BTN        A6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Range for SELECT Button (931 - 932)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945

int count = 0;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();
  Serial.println(F("NeoPixel Step Counter Ready"));
}

void loop() {
  int value = analogRead(BTN);

  // Exact SELECT detection
  bool selectPressed = (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX);

  // Clean single-press detection
  if (selectPressed && !lastButtonState && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    count++;
    if (count > LED_COUNT) {
      count = 0; // Reset after all 8 LEDs are lit
    }

    Serial.print(F("SELECT Pressed (Raw: "));
    Serial.print(value);
    Serial.print(F(") -> Count: "));
    Serial.println(count);

    strip.clear();

    // Turn ON LEDs sequentially up to current count
    for (int i = 0; i < count; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0)); // GREEN
    }
    strip.show();
  }

  lastButtonState = selectPressed;
}
