/*
====================================================
 Project-28 : IR Controlled Robot Car (Final Code)
====================================================
 Autobotix Robotics Board
 M11  -> D2 | TSOP -> D3 | M12 -> D4
 MFN  -> D6 (PWM) | M21 -> D7 | M22 -> D8
 RGB  -> D11 (Headlight - All 8 LEDs Working)
====================================================
*/

#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

//--------------- Motor Pins -----------------
#define M11 2
#define M12 4
#define MFN 6

#define M21 7
#define M22 8

//--------------- TSOP -----------------------
#define TSOP_PIN 3

//--------------- RGB Headlight --------------
#define RGB_PIN 11
#define NUMPIXELS 8

Adafruit_NeoPixel rgb(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

bool headlightState = false;
unsigned long lastHeadlightToggle = 0; // Debounce Timer

//--------------- Saved Custom Remote Commands ------------
#define POWER       0x12   // Emergency Stop

#define LOWSPD      0xA   // Button 1 (Low Speed)
#define MEDSPD      0x1B   // Button 2 (Medium Speed)
#define HIGHSPD     0x1F   // Button 3 (High Speed)

#define HEADLIGHT   0xC   // Button 4 (Headlight Toggle)

#define OK          0x1   // PAUSE Key (Car Stop)

#define UP          0x6   // VOL+ Key (Forward)
#define DOWN        0x5  // VOL- Key (Backward)
#define LEFT        0x2   // PREV Key (Left)
#define RIGHT       0x3   // NEXT Key (Right)

//--------------- Variables ------------------
int speedValue = 180;

//============================================

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);

  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);

  pinMode(MFN, OUTPUT);

  analogWrite(MFN, speedValue);

  // --- NeoPixel Setup (Fix for all 8 LEDs) ---
  rgb.begin();
  rgb.setBrightness(150); // Power stabilization for 8th LED
  
  // Turn OFF all 8 LEDs initially
  for (int i = 0; i < NUMPIXELS; i++) {
    rgb.setPixelColor(i, rgb.Color(0, 0, 0));
  }
  rgb.show();

  // --- TSOP Receiver Setup ---
  IrReceiver.begin(TSOP_PIN, ENABLE_LED_FEEDBACK);

  stopCar();

  Serial.println(F("=================================="));
  Serial.println(F(" AUTOBOTIX IR ROBOT CAR (CORRECT) "));
  Serial.println(F("=================================="));
  Serial.println(F("Ready..."));
}

//============================================

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.print(F("Forward  Speed : "));
  Serial.println(speedValue);
}

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.print(F("Backward Speed : "));
  Serial.println(speedValue);
}

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.print(F("Left Speed : "));
  Serial.println(speedValue);
}

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.print(F("Right Speed : "));
  Serial.println(speedValue);
}

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);

  Serial.println(F("Car Stopped"));
}

void emergencyStop()
{
  stopCar();

  Serial.println(F("***********************"));
  Serial.println(F(" EMERGENCY STOP "));
  Serial.println(F("***********************"));
}

//============================================

void toggleHeadlight()
{
  // 300ms Debounce Check (Prevents Flickering)
  if (millis() - lastHeadlightToggle < 300) {
    return; 
  }
  lastHeadlightToggle = millis();

  headlightState = !headlightState;

  if (headlightState)
  {
    // Force set all 8 LEDs to White
    for (int i = 0; i < NUMPIXELS; i++) {
      rgb.setPixelColor(i, rgb.Color(255, 255, 255));
    }
    Serial.println(F("Headlight ON (8 LEDs)"));
  }
  else
  {
    // Clear all 8 LEDs
    for (int i = 0; i < NUMPIXELS; i++) {
      rgb.setPixelColor(i, rgb.Color(0, 0, 0));
    }
    Serial.println(F("Headlight OFF"));
  }

  rgb.show();
}

//--------------- Speed Functions ----------------

void lowSpeed()
{
  speedValue = 80;
  analogWrite(MFN, speedValue);
  Serial.println(F("--------------------------"));
  Serial.println(F("Speed : LOW"));
  Serial.print(F("PWM   : "));
  Serial.println(speedValue);
  Serial.println(F("--------------------------"));
}

void mediumSpeed()
{
  speedValue = 180;
  analogWrite(MFN, speedValue);
  Serial.println(F("--------------------------"));
  Serial.println(F("Speed : MEDIUM"));
  Serial.print(F("PWM   : "));
  Serial.println(speedValue);
  Serial.println(F("--------------------------"));
}

void highSpeed()
{
  speedValue = 255;
  analogWrite(MFN, speedValue);
  Serial.println(F("--------------------------"));
  Serial.println(F("Speed : HIGH"));
  Serial.print(F("PWM   : "));
  Serial.println(speedValue);
  Serial.println(F("--------------------------"));
}

void printCommand(byte cmd)
{
  Serial.print(F("Command : 0x"));
  if(cmd < 0x10)
    Serial.print(F("0"));
  Serial.println(cmd, HEX);
}

//============================================

void loop()
{
  if (IrReceiver.decode())
  {
    // Filter out holding/repeat signals
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    byte cmd = IrReceiver.decodedIRData.command;

    printCommand(cmd);

    switch (cmd)
    {
      //---------------- Movement ----------------

      case UP:
        forward();
        break;

      case DOWN:
        backward();
        break;

      case LEFT:
        left();
        break;

      case RIGHT:
        right();
        break;

      case OK:
        stopCar();
        break;

      //---------------- Speed -------------------

      case LOWSPD:
        lowSpeed();
        break;

      case MEDSPD:
        mediumSpeed();
        break;

      case HIGHSPD:
        highSpeed();
        break;

      //---------------- Headlight ----------------

      case HEADLIGHT: // Button 4 Code (0x0C)
        toggleHeadlight();
        break;

      //---------------- Emergency Stop ----------

      case POWER: // Power Key Code (0x12)
        emergencyStop();
        break;

      //---------------- Default -----------------

      default:
        Serial.println(F("No Action Assigned"));
        break;
    }

    Serial.println();
    IrReceiver.resume();
  }
}
