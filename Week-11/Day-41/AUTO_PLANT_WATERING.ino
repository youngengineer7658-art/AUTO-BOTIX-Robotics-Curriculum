/*
==================================================
Day 41 | Project-41: Auto Plant Watering
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin     : D9
  - Soil Moisture Sensor: Pin A2 (Analog)
  - Button Ladder Pin   : Pin A6 (Calibrated: Select, Up, Down, Back)
  - Display             : 0.96" I2C OLED (U8g2 - RAM Safe)

Calibrated Button Values (A6):
  - Select (931-932) : Toggle Mode (AUTO <-> MANUAL)
  - Up     (853-855) : Manual Pump ON
  - Down   (789-790) : Manual Pump OFF
  - Back   (733-734) : Emergency STOP

Automation Logic:
  - Soil Dry (ADC > 600)  -> Auto Pump ON (Runs for safe burst)
  - Soil Wet (ADC <= 600) -> Auto Pump OFF
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN        9
#define SOIL_PIN        A2
#define BUTTONS_PIN     A6

// Calibrated Ladder Ranges
#define BTN_SEL_MIN     925
#define BTN_SEL_MAX     938
#define BTN_UP_MIN      848
#define BTN_UP_MAX      860
#define BTN_DOWN_MIN    783
#define BTN_DOWN_MAX    795
#define BTN_BACK_MIN    728
#define BTN_BACK_MAX    740

enum Button { BTN_NONE, BTN_SELECT, BTN_UP, BTN_DOWN, BTN_BACK };

Button readButton() {
  int val = analogRead(BUTTONS_PIN);
  if (val >= BTN_SEL_MIN  && val <= BTN_SEL_MAX)  return BTN_SELECT;
  if (val >= BTN_UP_MIN   && val <= BTN_UP_MAX)   return BTN_UP;
  if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX) return BTN_DOWN;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// System Configuration
const int DRY_THRESHOLD = 600; // Above 600 = Dry Soil
bool autoMode = true;          // Start in AUTO mode
bool pumpRunning = false;
int rawSoilValue = 0;
int moisturePercent = 0;

// Non-blocking Timers
unsigned long lastSensorRead = 0;
unsigned long lastSerialLog  = 0;
unsigned long pumpStartTime  = 0;
const unsigned long MAX_PUMP_BURST_MS = 5000; // 5 seconds safety cutoff

Button lastButton = BTN_NONE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setPump(bool state) {
  pumpRunning = state;
  digitalWrite(PUMP_PIN, pumpRunning ? HIGH : LOW);
  if (pumpRunning) {
    pumpStartTime = millis();
  }
}

void updateDisplay() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "AUTO PLANT WATER");
    u8g2.drawHLine(0, 15, 128);

    // Mode
    u8g2.drawStr(5, 30, "MODE :");
    u8g2.drawStr(55, 30, autoMode ? "AUTO" : "MANUAL");

    // Soil Moisture
    u8g2.drawStr(5, 46, "SOIL :");
    char moistBuf[6];
    itoa(moisturePercent, moistBuf, 10);
    u8g2.drawStr(55, 46, moistBuf);
    u8g2.drawStr(82, 46, "%");
    if (rawSoilValue > DRY_THRESHOLD) {
      u8g2.drawStr(100, 46, "[DRY]");
    } else {
      u8g2.drawStr(100, 46, "[OK]");
    }

    // Pump Status
    u8g2.drawStr(5, 61, "PUMP :");
    if (pumpRunning) {
      u8g2.drawStr(55, 61, "ON (WATERING)");
    } else {
      u8g2.drawStr(55, 61, "IDLE");
    }
  } while (u8g2.nextPage());
}

void handleButtonInputs() {
  Button currentBtn = readButton();

  if (currentBtn != lastButton && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();

    // Select: Toggle AUTO <-> MANUAL
    if (currentBtn == BTN_SELECT) {
      autoMode = !autoMode;
      setPump(false);
      updateDisplay();
      Serial.print(F("[MODE SWITCH] Current Mode: "));
      Serial.println(autoMode ? F("AUTO") : F("MANUAL"));
    }
    // Up: Manual Pump ON (Only in Manual Mode)
    else if (currentBtn == BTN_UP && !autoMode) {
      setPump(true);
      updateDisplay();
      Serial.println(F("[MANUAL] Pump Turned ON"));
    }
    // Down: Manual Pump OFF (Only in Manual Mode)
    else if (currentBtn == BTN_DOWN && !autoMode) {
      setPump(false);
      updateDisplay();
      Serial.println(F("[MANUAL] Pump Turned OFF"));
    }
    // Back: Emergency Stop (Works in both modes)
    else if (currentBtn == BTN_BACK) {
      autoMode = false;
      setPump(false);
      updateDisplay();
      Serial.println(F("[EMERGENCY STOP] Pump Cutoff & Set to MANUAL"));
    }
  }

  lastButton = currentBtn;
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(SOIL_PIN, INPUT);
  pinMode(BUTTONS_PIN, INPUT);

  setPump(false);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 41 | PROJECT-41: AUTO PLANT WATER  "));
  Serial.println(F("========================================"));
  Serial.println(F("Pump: D9 | Soil: A2 | Buttons: A6 (Ladder)"));

  updateDisplay();
}

void loop() {
  // 1. Read Button Inputs
  handleButtonInputs();

  // 2. Read Soil Moisture Sensor (Every 500ms)
  if (millis() - lastSensorRead >= 500) {
    lastSensorRead = millis();
    rawSoilValue = analogRead(SOIL_PIN);

    // Map raw ADC (1023 Dry -> 200 Wet) to percentage
    int mappedPercent = map(rawSoilValue, 1023, 200, 0, 100);
    moisturePercent = constrain(mappedPercent, 0, 100);

    // 3. Automation Logic in AUTO Mode
    if (autoMode) {
      if (rawSoilValue > DRY_THRESHOLD && !pumpRunning) {
        setPump(true); // Soil is dry -> start watering
        Serial.println(F("[AUTO] Soil is DRY -> Pump ON"));
      } 
      else if (rawSoilValue <= DRY_THRESHOLD && pumpRunning) {
        setPump(false); // Soil is moist enough -> stop watering
        Serial.println(F("[AUTO] Soil is MOIST -> Pump OFF"));
      }
    }

    // Safety timeout: Do not run continuously longer than safe burst limit
    if (pumpRunning && (millis() - pumpStartTime >= MAX_PUMP_BURST_MS)) {
      setPump(false);
      Serial.println(F("[SAFETY] Max watering burst reached -> Pump OFF"));
    }

    updateDisplay();
  }

  // 4. Live Telemetry on Serial Monitor (Every 1000ms)
  if (millis() - lastSerialLog >= 1000) {
    lastSerialLog = millis();
    Serial.print(F("Mode: "));
    Serial.print(autoMode ? F("AUTO  ") : F("MANUAL"));
    Serial.print(F(" | Raw Soil: "));
    Serial.print(rawSoilValue);
    Serial.print(F(" | Moisture: "));
    Serial.print(moisturePercent);
    Serial.print(F("% | Pump: "));
    Serial.println(pumpRunning ? F("ON (Watering)") : F("IDLE"));
  }
}