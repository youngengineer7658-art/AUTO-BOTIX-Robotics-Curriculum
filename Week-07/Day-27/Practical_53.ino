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
  Serial.println("       AUTOBOTIX IR READER        ");
  Serial.println("==================================");
  Serial.println("Press Any Button...");
  Serial.println();
}

String getButtonName(uint16_t command)
{
  switch (command)
  {
    // Numbers Mapping
    case 0xA: return "1";
    case 0x1B: return "2";
    case 0x1F: return "3";
    case 0xC: return "4";
    case 0xD: return "5";
    case 0xE: return "6";
    case 0x0: return "7";
    case 0xF: return "8";
    case 0x19: return "9"; // (Mapped as 9 from notes)

    // Controls & Power
    case 0x12: return "POWER";
    case 0x1E: return "MUTE";
    case 0x01: return "PAUSE";
    case 0x02: return "PREV (|<<)";
    case 0x03: return "NEXT (>>|)";

    // Functions
    case 0x04: return "EQ";
    case 0x05: return "VOL +";
    case 0x06: return "VOL -";
    case 0x08: return "RPT";
    case 0x09: return "U/SD";
    case 0x1A: return "MODE";

    default:   return "UNKNOWN";
  }
}

void loop()
{
  if (IrReceiver.decode())
  {
    // Repeat signals ko ignore karne ke liye filter
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
    {
      uint16_t command = IrReceiver.decodedIRData.command;

      Serial.print(getButtonName(command));
      Serial.print("   (Code: 0x");
      Serial.print(command, HEX);
      Serial.println(")");
    }

    IrReceiver.resume();
  }
}
