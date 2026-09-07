/*
==================================================
Smart Plant Watering System (Direct Sensor-Logic)
Board   : Young Engineer Board
Hardware:
  - Pump Output Pin     : D9
  - Soil Moisture Sensor: A2
  - Display             : 0.96" I2C OLED (U8g2 - RAM Safe)

Logic:
  - Mitti Sukhi (Moisture < Threshold) -> Pump ON (Pin D9 HIGH)
  - Mitti Geeli (Moisture >= Threshold)-> Pump OFF (Pin D9 LOW)
==================================================
*/

#include <Wire.h>
#include <U8g2lib.h>

#define PUMP_PIN 9
#define SOIL_PIN A2

// RAM-Safe U8g2 OLED Constructor
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Calibration Values (Resistive/Capacitive Sensor)
// Sukhi hawa me ~850, Paani me ~350
const int SOIL_DRY_RAW = 850;
const int SOIL_WET_RAW = 350;

// Moisture Threshold (35% se kam par sukhi, 35% ya zyada par geeli)
const int DRY_THRESHOLD_PERCENT = 35;

bool pumpState = false;

int getMoisturePercent() {
  int rawVal = analogRead(SOIL_PIN);
  // ADC mapping: zyada raw value = sukhi mitti, kam raw value = geeli mitti
  int percent = map(rawVal, SOIL_DRY_RAW, SOIL_WET_RAW, 0, 100);
  return constrain(percent, 0, 100);
}

void updateOLED(int moisture, bool isPumpOn) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 12, "SMART WATERING");
    u8g2.drawHLine(0, 15, 128);

    // Moisture Display
    u8g2.drawStr(5, 30, "MOISTURE :");
    char mBuf[6];
    itoa(moisture, mBuf, 10);
    u8g2.drawStr(75, 30, mBuf);
    u8g2.drawStr(95, 30, "%");

    // Soil Status
    u8g2.drawStr(5, 45, "SOIL     :");
    if (isPumpOn) {
      u8g2.drawStr(75, 45, "SUKHI");
    } else {
      u8g2.drawStr(75, 45, "GEELI");
    }

    // Motor State
    u8g2.drawStr(5, 60, "MOTOR    :");
    if (isPumpOn) {
      u8g2.drawStr(75, 60, "ON");
    } else {
      u8g2.drawStr(75, 60, "OFF");
    }
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(SOIL_PIN, INPUT);

  // Shuruat me pump band
  digitalWrite(PUMP_PIN, LOW);

  u8g2.begin();

  Serial.println(F("========================================"));
  Serial.println(F(" SMART PLANT WATERING SYSTEM STARTED    "));
  Serial.println(F("========================================"));
}

void loop() {
  int moisture = getMoisturePercent();

  // Core Logic: Mitti sukhi to ON, geeli to OFF
  if (moisture < DRY_THRESHOLD_PERCENT) {
    pumpState = true;
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    pumpState = false;
    digitalWrite(PUMP_PIN, LOW);
  }

  // Display aur Serial Update
  updateOLED(moisture, pumpState);

  Serial.print(F("Moisture: "));
  Serial.print(moisture);
  Serial.print(F("% | Soil: "));
  Serial.print(pumpState ? F("SUKHI") : F("GEELI"));
  Serial.print(F(" | Motor: "));
  Serial.println(pumpState ? F("ON") : F("OFF"));

  delay(500); // 0.5s stability delay
}
