#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define LED_COUNT 8
#define BTN       A6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Calibrated Ranges (Aapke Board ka Exact Data)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

int voteA = 0;
int voteB = 0;
int voteC = 0;

void showColor(uint32_t color) {
  strip.clear();
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  delay(300);

  strip.clear();
  strip.show();
}

void celebrateResult(uint32_t winColor) {
  // 3 baar winner color blink karega
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, winColor);
    }
    strip.show();
    delay(250);

    strip.clear();
    strip.show();
    delay(150);
  }
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.clear();
  strip.show();

  Serial.println(F("=================================="));
  Serial.println(F("       MINI VOTING MACHINE        "));
  Serial.println(F(" UP     : Candidate A (RED)       "));
  Serial.println(F(" DOWN   : Candidate B (GREEN)     "));
  Serial.println(F(" BACK   : Candidate C (BLUE)      "));
  Serial.println(F(" SELECT : Show Final Result       "));
  Serial.println(F("=================================="));
}

void loop() {
  int value = analogRead(BTN);

  // ---------------- Candidate A (UP: 853 - 855) ----------------
  if (value >= BTN_UP_MIN && value <= BTN_UP_MAX) {
    voteA++;
    Serial.print(F("Vote Cast -> Candidate A | Total: "));
    Serial.println(voteA);

    showColor(strip.Color(255, 0, 0)); // RED Flash

    while (analogRead(BTN) > 100);
    delay(150);
  }

  // ---------------- Candidate B (DOWN: 789 - 790) ----------------
  else if (value >= BTN_DOWN_MIN && value <= BTN_DOWN_MAX) {
    voteB++;
    Serial.print(F("Vote Cast -> Candidate B | Total: "));
    Serial.println(voteB);

    showColor(strip.Color(0, 255, 0)); // GREEN Flash

    while (analogRead(BTN) > 100);
    delay(150);
  }

  // ---------------- Candidate C (BACK: 733 - 734) ----------------
  else if (value >= BTN_BACK_MIN && value <= BTN_BACK_MAX) {
    voteC++;
    Serial.print(F("Vote Cast -> Candidate C | Total: "));
    Serial.println(voteC);

    showColor(strip.Color(0, 0, 255)); // BLUE Flash

    while (analogRead(BTN) > 100);
    delay(150);
  }

  // ---------------- Declare Result (SELECT: 931 - 932) ----------------
  else if (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX) {
    Serial.println();
    Serial.println(F("========== FINAL RESULT =========="));
    Serial.print(F("Candidate A (RED)   : ")); Serial.println(voteA);
    Serial.print(F("Candidate B (GREEN) : ")); Serial.println(voteB);
    Serial.print(F("Candidate C (BLUE)  : ")); Serial.println(voteC);
    Serial.println(F("----------------------------------"));

    if (voteA > voteB && voteA > voteC) {
      Serial.println(F("WINNER : Candidate A (RED)"));
      celebrateResult(strip.Color(255, 0, 0));
    } 
    else if (voteB > voteA && voteB > voteC) {
      Serial.println(F("WINNER : Candidate B (GREEN)"));
      celebrateResult(strip.Color(0, 255, 0));
    } 
    else if (voteC > voteA && voteC > voteB) {
      Serial.println(F("WINNER : Candidate C (BLUE)"));
      celebrateResult(strip.Color(0, 0, 255));
    } 
    else {
      Serial.println(F("RESULT : TIE / DRAW"));
      celebrateResult(strip.Color(255, 255, 255)); // White on Tie
    }
    Serial.println(F("=================================="));

    while (analogRead(BTN) > 100);
    delay(150);
  }
}
