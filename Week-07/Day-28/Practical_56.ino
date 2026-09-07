/*
==============================================
Practical-56 : Speed Control via IR
Autobotix Robotics Board

TSOP  -> D3

M11 -> D2
M12 -> D4
MFN -> D6 (PWM)

M21 -> D7
M22 -> D8
==============================================
*/

#include <IRremote.hpp>

#define M11 2
#define M12 4
#define MFN 6

#define M21 7
#define M22 8

#define TSOP_PIN 3

// Movement Commands
#define UP      0x0E
#define DOWN    0x1A
#define LEFT    0x0A
#define RIGHT   0x1E
#define OK      0x05

// Speed Commands
#define LOW_SPEED     0x0D     // Button 1
#define MEDIUM_SPEED  0x19     // Button 2
#define HIGH_SPEED    0x1B     // Button 3

int speedValue = 180;

//================================

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  analogWrite(MFN, speedValue);

  IrReceiver.begin(TSOP_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("==============================");
  Serial.println(" IR SPEED CONTROL ");
  Serial.println("==============================");
  Serial.println("Default Speed : Medium");
}

//================================

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.print("Forward  Speed : ");
  Serial.println(speedValue);
}

//================================

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.print("Backward Speed : ");
  Serial.println(speedValue);
}

//================================

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.print("Left Speed : ");
  Serial.println(speedValue);
}

//================================

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.print("Right Speed : ");
  Serial.println(speedValue);
}

//================================

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);

  Serial.println("Car Stopped");
}

//================================

void loop()
{
  if (IrReceiver.decode())
  {
    byte cmd = IrReceiver.decodedIRData.command;

    if (cmd != 0x0)
    {
      switch (cmd)
      {
        //--------- Speed -------------

        case LOW_SPEED:
          speedValue = 80;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : LOW");
          break;

        case MEDIUM_SPEED:
          speedValue = 180;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : MEDIUM");
          break;

        case HIGH_SPEED:
          speedValue = 255;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : HIGH");
          break;

        //--------- Movement ----------

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
      }
    }

    IrReceiver.resume();
  }
}
