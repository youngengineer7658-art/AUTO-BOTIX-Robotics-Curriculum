/*
==========================================
Project-23 : Bluetooth RC Car
==========================================

HC-05 TX -> D0
HC-05 RX -> D1

Verified Board Mapping
M11 -> D2
M12 -> D4
MFN -> D6 (PWM)
M21 -> D7
M22 -> D8
BUZZER -> D12

RGB LED -> D11 (WS2812B)

Gamepad Button Mapping (App Verified):
D-pad    = F / B / L / R
Square   = S (Stop)
Circle   = C (Headlight ON)
X        = X (Headlight OFF)
Triangle = T (Horn)
==========================================
*/

#include <Adafruit_NeoPixel.h>

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8
#define BUZZER 12

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
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();

  analogWrite(MFN, speedValue);

  stopCar();

  Serial.println("==================================");
  Serial.println(" Bluetooth RC Car Ready ");
  Serial.println("==================================");
  Serial.println("F = Forward");
  Serial.println("B = Backward");
  Serial.println("L = Left");
  Serial.println("R = Right");
  Serial.println("S = Stop");
  Serial.println("+ = Speed Up");
  Serial.println("- = Speed Down");
  Serial.println("C = Headlight ON");
  Serial.println("X = Headlight OFF");
  Serial.println("T = Horn");
}

//==========================
void forward()
//==========================
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  Serial.println("FORWARD");
}

//==========================
void backward()
//==========================
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  Serial.println("BACKWARD");
}

//==========================
void left()
//==========================
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  Serial.println("LEFT");
}

//==========================
void right()
//==========================
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  Serial.println("RIGHT");
}

//==========================
void stopCar()
//==========================
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  Serial.println("STOP");
}

//==========================
void headLight(bool state)
//==========================
{
  if (state)
  {
    for (int i = 0; i < NUMPIXELS; i++)
      strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  else
  {
    strip.clear();
  }

  strip.show();
}

//==========================
void horn()
//==========================
{
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  Serial.println("HORN");
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
        break;

      case 'B':
        backward();
        break;

      case 'L':
        left();
        break;

      case 'R':
        right();
        break;

      case 'S':
        stopCar();
        break;

      case '+':
        if (speedValue < 255)
          speedValue += 25;

        analogWrite(MFN, speedValue);

        Serial.print("Speed = ");
        Serial.println(speedValue);
        break;

      case '-':
        if (speedValue > 25)
          speedValue -= 25;

        analogWrite(MFN, speedValue);

        Serial.print("Speed = ");
        Serial.println(speedValue);
        break;

      case 'H':
      case 'C':                    // Circle button -> Headlight ON
        headLight(true);
        Serial.println("Headlight ON");
        break;

      case 'X':                    // X button -> Headlight OFF
        headLight(false);
        Serial.println("Headlight OFF");
        break;

      case 'T':                    // Triangle button -> Horn
        horn();
        break;

      case '0':                    // Button release signal - ignore
        break;

      default:
        Serial.println("Invalid Command");
        break;
    }
  }
}