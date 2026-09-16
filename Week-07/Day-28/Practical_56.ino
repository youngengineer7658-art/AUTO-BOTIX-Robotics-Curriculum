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

//------------- Updated Saved IR Commands -----

// Movement Commands
#define UP        0x6   // Mapped from VOL+ (Forward)
#define DOWN      0x5   // Mapped from VOL- (Backward)
#define LEFT      0x2   // Mapped from PREV (|<<) (Turn Left)
#define RIGHT     0x3   // Mapped from NEXT (>>|) (Turn Right)
#define OK        0x1   // Mapped from PAUSE (Stop Car)

// Speed Commands
#define LOW_SPEED     0xA   // Button 1 (Low Speed - 80)
#define MEDIUM_SPEED  0x1B   // Button 2 (Medium Speed - 180)
#define HIGH_SPEED    0x1F   // Button 3 (High Speed - 255)

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
        //--------- Speed Controls ----------

        case LOW_SPEED:
          speedValue = 80;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : LOW (80)");
          break;

        case MEDIUM_SPEED:
          speedValue = 180;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : MEDIUM (180)");
          break;

        case HIGH_SPEED:
          speedValue = 255;
          analogWrite(MFN, speedValue);
          Serial.println("Speed : HIGH (255)");
          break;

        //--------- Movement Controls -------

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
