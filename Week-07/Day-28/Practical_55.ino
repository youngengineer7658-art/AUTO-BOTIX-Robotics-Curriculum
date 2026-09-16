/*
==============================================
Practical-55 : Car Control with IR
Autobotix Robotics Board

TSOP  -> D3

M11 -> D2
M12 -> D4
MFN -> D6

M21 -> D7
M22 -> D8
==============================================
*/

#include <IRremote.hpp>

//------------- Motor Pins ----------------

#define M11 2
#define M12 4
#define MFN 6

#define M21 7
#define M22 8

//------------- Updated Saved IR Commands -----

#define TSOP_PIN 3

#define UP      0x6   // Mapped from VOL+ (Forward)
#define DOWN    0x5   // Mapped from VOL- (Backward)
#define LEFT    0x2   // Mapped from PREV (|<<) (Turn Left)
#define RIGHT   0x3   // Mapped from NEXT (>>|) (Turn Right)
#define OK      0x1   // Mapped from PAUSE (Stop Car)

//-----------------------------------------

int speedValue = 180;

unsigned long lastSignalTime = 0;
const unsigned long STOP_TIMEOUT = 200;   // ms - signal rukte hi auto-stop
bool isMoving = false;

//=========================================

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
  Serial.println(" AUTOBOTIX IR CAR CONTROL ");
  Serial.println("==============================");
  Serial.println("Waiting for Command...");
}

//=========================================

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.println("Forward");
}

//=========================================

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.println("Backward");
}

//=========================================

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, speedValue);

  Serial.println("Left");
}

//=========================================

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, speedValue);

  Serial.println("Right");
}

//=========================================

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);

  Serial.println("Stop");
}

//=========================================

void loop()
{
  if (IrReceiver.decode())
  {
    byte cmd = IrReceiver.decodedIRData.command;

    if (cmd != 0x0)   // Glitch reading ignore karo
    {
      lastSignalTime = millis();

      Serial.print("Command : 0x");
      Serial.println(cmd, HEX);

      switch (cmd)
      {
        case UP:
          forward();
          isMoving = true;
          break;

        case DOWN:
          backward();
          isMoving = true;
          break;

        case LEFT:
          left();
          isMoving = true;
          break;

        case RIGHT:
          right();
          isMoving = true;
          break;

        case OK:
          stopCar();
          isMoving = false;
          break;

        default:
          Serial.println("No Action");
          break;
      }
    }

    IrReceiver.resume();
  }

  // Agar button release kar diya, toh 200ms baad car automatic stop ho jayegi
  if (isMoving && (millis() - lastSignalTime > STOP_TIMEOUT))
  {
    stopCar();
    isMoving = false;
  }
}
