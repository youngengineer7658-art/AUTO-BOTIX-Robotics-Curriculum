/*
==================================================
Day 44 | Practical-87: Debug Pump Control System
Board  : Young Engineer Board
Hardware:
  - Pump Output Pin   : D9
  - Button Ladder Pin : A6 (Calibrated Values)
  - Display           : 0.96" I2C OLED (U8g2 - RAM Safe)

Debugging Diagnostics:
  - Raw ADC Telemetry for Button Ladder (Pin A6)
  - Pin D9 Output Logic State Verification (HIGH/LOW)
  - Pump Pulse Test via Select Button (2-Second Burst)
  - Instant Cutoff via Back Button
  - Serial Diagnostic Logging (ADC value + Voltage + State)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN    9
#define BUTTONS_PIN A6

// Calibrated Ladder Ranges
#define BTN_SEL_MIN  925
#define BTN_SEL_MAX  938
#define BTN_UP_MIN   848
#define BTN_UP_MAX   860
#define BTN_DOWN_MIN 783
#define BTN_DOWN_MAX 795
#define BTN_BACK_MIN 728
#define BTN_BACK_MAX 740

enum Button { BTN_NONE, BTN_SELECT, BTN_UP, BTN_DOWN, BTN_BACK };

Button identifyButton(int adc) {
  if (adc >= BTN_SEL_MIN  && adc <= BTN_SEL_MAX)  return BTN_SELECT;
  if (adc >= BTN_UP_MIN   && adc <= BTN_UP_MAX)   return BTN_UP;
  if (adc >= BTN_DOWN_MIN && adc <= BTN_DOWN_MAX) return BTN_DOWN;
  if (adc >= BTN_BACK_MIN && adc <= BTN_BACK_MAX) return BTN_BACK;
  return BTN_NONE;
}

const char* getButtonLabel(Button b) {
  switch (b) {
    case BTN_SELECT: return "SELECT";
    case BTN_UP:     return "UP";
    case BTN_DOWN:   return "DOWN";
    case BTN_BACK:   return "BACK";
    default:         return "IDLE";
  }
}

// RAM-Safe OLED Constructor (128 bytes buffer)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Diagnostics Variables
bool pumpOutputState = false;
unsigned long pulseStartTime = 0;
const unsigned long PULSE_DURATION_MS = 2000; // 2s debug test pulse

unsigned long lastSerialLog = 0;
const unsigned long serialInterval = 500; // 500ms log rate

void setPump(bool state) {
  pumpOutputState = state;
  digitalWrite(PUMP_PIN, pumpOutputState ? HIGH : LOW);
  if (pumpOutputState) {
    pulseStartTime = millis();
  }
}

void renderDiagnostics(int rawAdc, Button btn) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "PRACTICAL-87: DEBUG");
    u8g2.drawHLine(0, 15, 128);

    // Line 1: Raw ADC & Identified Button
    u8g2.drawStr(5, 29, "A6 ADC :");
    char adcBuf[6];
    itoa(rawAdc, adcBuf, 10);
    u8g2.drawStr(60, 29, adcBuf);

    u8g2.drawStr(5, 43, "KEY    :");
    u8g2.drawStr(60, 43, getButtonLabel(btn));

    // Line 2: D9 Pin Logic Verification
    u8g2.drawStr(5, 57, "PIN D9 :");
    if (pumpOutputState) {
      u8g2.drawStr(60, 57, "HIGH (ON)");
    } else {
      u8g2.drawStr(60, 57, "LOW (OFF)");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTONS_PIN, INPUT);

  setPump(false); // Default safe boot state

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" DAY 44 | PRACTICAL-87: DEBUG PUMP CTRL "));
  Serial.println(F("========================================"));
  Serial.println(F("Tests: SEL (2s Pulse) | BACK (Instant Cutoff)"));
  Serial.println(F("Monitoring: A6 Raw ADC + D9 Logic Output"));
}

void loop() {
  // 1. Read Raw Telemetry
  int rawAdc = analogRead(BUTTONS_PIN);
  Button btn = identifyButton(rawAdc);

  // 2. Action Trigger for Debugging
  if (btn == BTN_SELECT && !pumpOutputState) {
    setPump(true);
    Serial.println(F("[DEBUG ACTION] SELECT detected -> Starting 2s Pulse Test"));
  } else if (btn == BTN_BACK && pumpOutputState) {
    setPump(false);
    Serial.println(F("[DEBUG ACTION] BACK detected -> Instant Cutoff Applied"));
  }

  // 3. Non-blocking Pulse Safety Timeout
  if (pumpOutputState && (millis() - pulseStartTime >= PULSE_DURATION_MS)) {
    setPump(false);
    Serial.println(F("[DEBUG] 2s Pulse Test Complete -> D9 returned to LOW"));
  }

  // 4. OLED Diagnostics Refresh
  renderDiagnostics(rawAdc, btn);

  // 5. Serial Logging Stream (every 500ms)
  if (millis() - lastSerialLog >= serialInterval) {
    lastSerialLog = millis();
    float pinVoltage = (rawAdc * 5.0) / 1023.0;

    Serial.print(F("[DIAG] ADC: "));
    Serial.print(rawAdc);
    Serial.print(F(" | Volt: "));
    Serial.print(pinVoltage, 2);
    Serial.print(F("V | Btn: "));
    Serial.print(getButtonLabel(btn));
    Serial.print(F(" | D9 Output: "));
    Serial.println(pumpOutputState ? F("HIGH") : F("LOW"));
  }
}