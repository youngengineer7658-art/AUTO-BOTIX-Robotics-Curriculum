/*
==================================================
Practical-70 : Remote Controlled Motor
==================================================

Objective:
To control the robot motors wirelessly
using an IR Remote.

Hardware Required:
• Autobotix Robotics Board
• IR Remote
• Onboard TSOP IR Receiver

Pin Mapping:

TSOP Receiver -> D3

M11 -> D2
M12 -> D4
MFN -> D6

M21 -> D7
M22 -> D8

Button Mapping:

▲  -> Forward
▼  -> Backward
◀  -> Left
▶  -> Right
OK -> Stop

==================================================
*/

#include <IRremote.hpp>

//------------- Motor Pins ----------------

#define M11 2
#define M12 4
#define MFN 6

#define M21 7
#define M22 8

//------------- IR Receiver ----------------

#define IR_PIN 3

//------------- Remote Commands ------------

#define UP      0x0E
#define DOWN    0x1A
#define LEFT    0x0A
#define RIGHT   0x1E
#define OK      0x05

//------------------------------------------

int speedValue = 180;

//==========================================

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  stopMotor();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("==================================");
  Serial.println("Practical-70 : Remote Controlled Motor");
  Serial.println("==================================");
  Serial.println("Waiting for IR Command...");
}

//==========================================

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.println("Forward");
}

//==========================================

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.println("Backward");
}

//==========================================

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.println("Left");
}

//==========================================

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.println("Right");
}

//==========================================

void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);

  Serial.println("Stop");
}

//==========================================

void loop()
{
  if (IrReceiver.decode())
  {
    byte cmd = IrReceiver.decodedIRData.command;

    Serial.print("Command : 0x");
    Serial.println(cmd, HEX);

    switch (cmd)
    {
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
        stopMotor();
        break;

      default:
        Serial.println("No Action");
        break;
    }

    Serial.println("--------------------------");

    IrReceiver.resume();
  }
}