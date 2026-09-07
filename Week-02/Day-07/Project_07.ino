#include <Adafruit_NeoPixel.h>

#define LED_PIN     11
#define NUM_LEDS    8
#define BUTTON_PIN  A6
#define POT_PIN     A7

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Range for UP Button (853 - 855)
#define BTN_UP_MIN 845
#define BTN_UP_MAX 865

int colorIndex = 0;
bool buttonPressed = false;
int lastBrightness = -1;

const char* colorNames[7] = {"Red", "Green", "Blue", "Yellow", "Purple", "Cyan", "White"};
uint32_t colors[7];

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();

  colors[0] = strip.Color(255, 0, 0);     // Red
  colors[1] = strip.Color(0, 255, 0);     // Green
  colors[2] = strip.Color(0, 0, 255);     // Blue
  colors[3] = strip.Color(255, 255, 0);   // Yellow
  colors[4] = strip.Color(255, 0, 255);   // Purple
  colors[5] = strip.Color(0, 255, 255);   // Cyan
  colors[6] = strip.Color(255, 255, 255); // White

  Serial.println(F("===== MOOD LAMP STARTED ====="));
  Serial.println(F("Press UP Button to Cycle Colors | Rotate Pot to adjust Brightness"));
}

void updateLamp() {
  int potValue = analogRead(POT_PIN);
  int brightness = map(potValue, 0, 1023, 0, 255);

  // Sirf tab update karega jab potentiometer ghoomega
  if (abs(brightness - lastBrightness) > 2) {
    lastBrightness = brightness;
    strip.setBrightness(brightness);

    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, colors[colorIndex]);
    }
    strip.show();
  }
}

void loop() {
  updateLamp();

  int value = analogRead(BUTTON_PIN);

  // UP Button Detection (853 - 855)
  if (value >= BTN_UP_MIN && value <= BTN_UP_MAX) {
    if (!buttonPressed) {
      colorIndex++;
      if (colorIndex >= 7) colorIndex = 0;

      // Force instant color update
      strip.setBrightness(lastBrightness);
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, colors[colorIndex]);
      }
      strip.show();

      Serial.print(F("Raw: "));
      Serial.print(value);
      Serial.print(F(" | Color: "));
      Serial.println(colorNames[colorIndex]);

      buttonPressed = true;
    }
  } else {
    buttonPressed = false;
  }

  delay(20);
}
