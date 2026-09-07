/*
==========================================
Practical-53 : Read IR Codes (Named)
Autobotix Robotics Board

TSOP IR Receiver -> D3
==========================================
*/

#include <IRremote.hpp>

#define IR_PIN 3

void setup()
{
  Serial.begin(9600);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("==================================");
  Serial.println("      AUTOBOTIX IR READER");
  Serial.println("==================================");
  Serial.println("Press Any Button...");
  Serial.println();
}

String getButtonName(uint16_t command)
{
  switch (command)
  {
    case 0xE:  return "UP";
    case 0x1A: return "DOWN";
    case 0xA:  return "LEFT";
    case 0x1E: return "RIGHT";
    case 0x5:  return "OK";

    case 0x16: return "0";
    case 0x0:  return "1";
    case 0x19: return "2";
    case 0x1B: return "3";
    case 0x1:  return "4";
    case 0x11: return "5";
    case 0x15: return "6";
    case 0x17: return "7";
    case 0x6:  return "8";
    case 0x12: return "9";

    case 0x4C: return "SIGNAL";
    case 0x54: return "PAUSE";
    case 0x9:  return "POWER OFF";

    default:   return "UNKNOWN";
  }
}

void loop()
{
  if (IrReceiver.decode())
  {
    uint16_t command = IrReceiver.decodedIRData.command;

    Serial.print(getButtonName(command));
    Serial.print("   (Code: 0x");
    Serial.print(command, HEX);
    Serial.println(")");

    IrReceiver.resume();
  }
}
