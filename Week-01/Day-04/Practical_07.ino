#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define LED_COUNT 8
#define BTN       A6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated ADC Ranges (Aapke Board ka Data)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

enum ButtonState {
  STATE_NONE,
  STATE_UP_RED,
  STATE_DOWN_GREEN,
  STATE_BACK_BLUE,
  STATE_SELECT_WHITE
};

ButtonState lastState = STATE_NONE;
ButtonState candidateState = STATE_NONE;
unsigned long candidateStartTime = 0;
const unsigned long DEBOUNCE_DELAY = 150;

int readAveraged() {
  long sum = 0;
  int samples = 10;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(BTN);
    delay(2);
  }
  return sum / samples;
}

void setAllLeds(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();
  Serial.println(F("Calibrated Keypad Color Controller Ready"));
}

void loop() {
  int value = readAveraged();
  ButtonState state = STATE_NONE;

  // Exact Value Matching
  if (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX) {
    state = STATE_SELECT_WHITE;
  }
  else if (value >= BTN_UP_MIN && value <= BTN_UP_MAX) {
    state = STATE_UP_RED;
  }
  else if (value >= BTN_DOWN_MIN && value <= BTN_DOWN_MAX) {
    state = STATE_DOWN_GREEN;
  }
  else if (value >= BTN_BACK_MIN && value <= BTN_BACK_MAX) {
    state = STATE_BACK_BLUE;
  }
  else {
    state = STATE_NONE;
  }

  // Debounce logic
  if (state != candidateState) {
    candidateState = state;
    candidateStartTime = millis();
  }

  if ((millis() - candidateStartTime) > DEBOUNCE_DELAY) {
    if (candidateState != lastState) {

      switch (candidateState) {
        case STATE_UP_RED:
          setAllLeds(255, 0, 0); // RED
          Serial.print(F("Raw: "));
          Serial.print(value);
          Serial.println(F(" | State: UP - RED"));
          break;

        case STATE_DOWN_GREEN:
          setAllLeds(0, 255, 0); // GREEN
          Serial.print(F("Raw: "));
          Serial.print(value);
          Serial.println(F(" | State: DOWN - GREEN"));
          break;

        case STATE_BACK_BLUE:
          setAllLeds(0, 0, 255); // BLUE
          Serial.print(F("Raw: "));
          Serial.print(value);
          Serial.println(F(" | State: BACK - BLUE"));
          break;

        case STATE_SELECT_WHITE:
          setAllLeds(255, 255, 255); // WHITE
          Serial.print(F("Raw: "));
          Serial.print(value);
          Serial.println(F(" | State: SELECT - WHITE"));
          break;

        case STATE_NONE:
        default:
          strip.clear();
          strip.show();
          Serial.print(F("Raw: "));
          Serial.print(value);
          Serial.println(F(" | State: RELEASED (OFF)"));
          break;
      }

      lastState = candidateState;
    }
  }
}
