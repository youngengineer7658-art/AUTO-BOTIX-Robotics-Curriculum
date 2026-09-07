/*
==========================================
Practical-48 : Debug Bluetooth Commands
==========================================

HC-05 TX -> D0
HC-05 RX -> D1

Verified Board Mapping
M11 -> D2
M12 -> D4
MFN -> D6 (PWM)
M21 -> D7
M22 -> D8

RGB LED -> D11 (WS2812B)
==========================================
*/

#include <Adafruit_NeoPixel.h>

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

#define RGB_PIN 11
#define NUMPIXELS 8

Adafruit_NeoPixel strip(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

int speedValue = 180;

//==========================
void setup()
//==========================
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();

  analogWrite(MFN, speedValue);

  stopCar();

  Serial.println("==================================");
  Serial.println(" BLUETOOTH DEBUG MODE ");
  Serial.println("==================================");
  Serial.println("Format: Time | Char | ASCII | Action");
}

//==========================
void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);
  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
}

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void headLight(bool state)
{
  if (state) {
    for (int i = 0; i < NUMPIXELS; i++)
      strip.setPixelColor(i, strip.Color(255, 255, 255));
  } else {
    strip.clear();
  }
  strip.show();
}

//==========================
void printDebug(char c, String action)
//==========================
{
  Serial.print(millis());
  Serial.print(" ms | Char: ");
  Serial.print(c);
  Serial.print(" | ASCII: ");
  Serial.print((int)c);
  Serial.print(" | Action: ");
  Serial.println(action);
}

//==========================
void loop()
//==========================
{
  if (Serial.available())
  {
    char cmd = toupper(Serial.read());

    switch (cmd)
    {
      case 'F':
        forward();
        printDebug(cmd, "FORWARD");
        break;

      case 'B':
        backward();
        printDebug(cmd, "BACKWARD");
        break;

      case 'L':
        left();
        printDebug(cmd, "LEFT");
        break;

      case 'R':
        right();
        printDebug(cmd, "RIGHT");
        break;

      case 'S':
        stopCar();
        printDebug(cmd, "STOP");
        break;

      case '+':
        if (speedValue < 255)
          speedValue += 25;
        analogWrite(MFN, speedValue);
        printDebug(cmd, "SPEED UP -> " + String(speedValue));
        break;

      case '-':
        if (speedValue > 25)
          speedValue -= 25;
        analogWrite(MFN, speedValue);
        printDebug(cmd, "SPEED DOWN -> " + String(speedValue));
        break;

      case 'H':
      case 'C':
        headLight(true);
        printDebug(cmd, "HEADLIGHT ON");
        break;

      case 'X':
        headLight(false);
        printDebug(cmd, "HEADLIGHT OFF");
        break;

      case '0':
      case 'T':
        printDebug(cmd, "IGNORED (release/free button)");
        break;

      default:
        printDebug(cmd, "INVALID COMMAND");
        break;
    }
  }
}