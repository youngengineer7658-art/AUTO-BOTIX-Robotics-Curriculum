/*
==================================================
Home Automation Demo (U8g2 Stable - Zero Memory Issue)
Board: Auto-Botix / Young Engineer Board
==================================================
*/

#define DECODE_NEC

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include <IRremote.hpp>

// 1. PIN DEFINITIONS
#define IR_PIN     3
#define RGB_PIN    11
#define NUM_LEDS   8
#define BUZZER_PIN 12
#define TRIG_PIN   5
#define ECHO_PIN   A3
#define BUTTON_PIN A6

// 2. HARDWARE OBJECTS (U8g2 Page Buffer takes only 128 bytes RAM)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel rgb(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// 3. CALIBRATED ADC BUTTON RANGES
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

// 4. REMOTE COMMANDS
#define POWER   0x09
#define BTN1    0x0D
#define BTN2    0x19
#define BTN3    0x1B
#define OK      0x05

// 5. GLOBAL VARIABLES
bool alarmOn = false;
bool autoMode = false;
const char* lightStatus = "OFF";
int distance = 999;
int lastBtn = 0;

unsigned long lastBuzzerTime = 0;
bool buzzerState = false;
unsigned long lastDisplayTime = 0;
unsigned long lastSensorTime = 0;
unsigned long lastSerialTime = 0;

void showColor(uint8_t r, uint8_t g, uint8_t b)
{
  for (int i = 0; i < NUM_LEDS; i++)
    rgb.setPixelColor(i, rgb.Color(r, g, b));
  rgb.show();
}

void rgbOff()
{
  rgb.clear();
  rgb.show();
}

int getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

int readButton()
{
  int val = analogRead(BUTTON_PIN);
  if (val >= BTN_SELECT_MIN && val <= BTN_SELECT_MAX) return 4;
  if (val >= BTN_UP_MIN && val <= BTN_UP_MAX)         return 1;
  if (val >= BTN_DOWN_MIN && val <= BTN_DOWN_MAX)     return 2;
  if (val >= BTN_BACK_MIN && val <= BTN_BACK_MAX)     return 3;
  return 0;
}

void updateOLED()
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 10, "HOME AUTOMATION");
    u8g2.drawHLine(0, 13, 128);

    u8g2.drawStr(5, 26, "LIGHT:");
    u8g2.drawStr(55, 26, lightStatus);

    u8g2.drawStr(5, 38, "ALARM:");
    u8g2.drawStr(55, 38, alarmOn ? "ON" : "OFF");

    u8g2.drawStr(5, 50, "MODE :");
    u8g2.drawStr(55, 50, autoMode ? "AUTO" : "MANUAL");

    u8g2.drawStr(5, 62, "DIST :");
    if (distance >= 400 || distance <= 0 || distance == 999) {
      u8g2.drawStr(55, 62, "--");
    } else {
      char distBuf[8];
      itoa(distance, distBuf, 10);
      u8g2.drawStr(55, 62, distBuf);
      u8g2.drawStr(85, 62, "cm");
    }
  } while (u8g2.nextPage());
}

void handleAction(byte action)
{
  switch (action)
  {
    case 0:
      rgbOff();
      alarmOn = false;
      autoMode = false;
      lightStatus = "OFF";
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println(F("[ACTION] ALL OFF"));
      break;

    case 1:
      autoMode = false;
      showColor(255, 255, 255);
      lightStatus = "LIVING";
      Serial.println(F("[ACTION] LIVING ROOM LIGHT ON"));
      break;

    case 2:
      autoMode = false;
      showColor(255, 160, 60);
      lightStatus = "BEDROOM";
      Serial.println(F("[ACTION] BEDROOM LIGHT ON"));
      break;

    case 3:
      alarmOn = !alarmOn;
      Serial.print(F("[ACTION] ALARM: "));
      Serial.println(alarmOn ? F("ON") : F("OFF"));
      if (!alarmOn) digitalWrite(BUZZER_PIN, LOW);
      break;

    case 4:
      autoMode = !autoMode;
      Serial.print(F("[ACTION] AUTO MODE: "));
      Serial.println(autoMode ? F("ON") : F("OFF"));
      if (!autoMode) {
        rgbOff();
        lightStatus = "OFF";
      }
      break;
  }
  updateOLED();
}

void setup()
{
  Serial.begin(9600);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize U8g2 OLED
  u8g2.begin();
  Serial.println(F("OLED : OK (U8g2)"));

  rgb.begin();
  rgbOff();

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

  Serial.println(F("=================================="));
  Serial.println(F("    HOME AUTOMATION READY         "));
  Serial.println(F("=================================="));

  updateOLED();
}

void loop()
{
  // 1. IR Remote
  if (IrReceiver.decode())
  {
    byte cmd = IrReceiver.decodedIRData.command;
    if (cmd != 0x0)
    {
      Serial.print(F("[IR RECV] Code: 0x"));
      Serial.println(cmd, HEX);

      if (cmd == POWER) handleAction(0);
      else if (cmd == BTN1) handleAction(1);
      else if (cmd == BTN2) handleAction(2);
      else if (cmd == BTN3) handleAction(3);
      else if (cmd == OK)   handleAction(4);
    }
    IrReceiver.resume();
  }

  // 2. Hardware Buttons (A6)
  int btn = readButton();
  if (btn != 0 && lastBtn == 0)
  {
    Serial.print(F("[BTN PRESS] Key: "));
    if (btn == 1)      { Serial.println(F("UP (Living)"));   handleAction(1); }
    else if (btn == 2) { Serial.println(F("DOWN (Bedroom)")); handleAction(2); }
    else if (btn == 3) { Serial.println(F("BACK (Alarm)"));   handleAction(3); }
    else if (btn == 4) { Serial.println(F("SELECT (Auto)"));  handleAction(4); }
  }
  lastBtn = btn;

  // 3. Occupancy Sensor in Auto Mode
  if (autoMode && (millis() - lastSensorTime >= 150))
  {
    lastSensorTime = millis();
    distance = getDistance();

    if (distance <= 50 && distance > 0) {
      showColor(0, 255, 0);
      lightStatus = "AUTO-ON";
    } else {
      rgbOff();
      lightStatus = "AUTO-OFF";
    }
  }

  // 4. Non-blocking Alarm Beep
  if (alarmOn)
  {
    if (millis() - lastBuzzerTime >= 150)
    {
      lastBuzzerTime = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }
  }

  // 5. Periodic OLED Refresh (Every 250ms)
  if (millis() - lastDisplayTime >= 250)
  {
    lastDisplayTime = millis();
    updateOLED();
  }

  // 6. Periodic Serial Monitor Telemetry (Every 1000ms)
  if (millis() - lastSerialTime >= 1000)
  {
    lastSerialTime = millis();
    Serial.print(F("TELEMETRY | Mode: "));
    Serial.print(autoMode ? F("AUTO  ") : F("MANUAL"));
    Serial.print(F(" | Light: "));
    Serial.print(lightStatus);
    Serial.print(F("\t | Alarm: "));
    Serial.print(alarmOn ? F("ON ") : F("OFF"));
    Serial.print(F(" | Dist: "));
    if (distance >= 400 || distance <= 0 || distance == 999) {
      Serial.println(F("--"));
    } else {
      Serial.print(distance);
      Serial.println(F(" cm"));
    }
  }
}
