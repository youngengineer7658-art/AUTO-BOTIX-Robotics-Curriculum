/*
==========================================
Project-27 : IR Remote Switch
==========================================
POWER  -> All OFF
1      -> RGB RED
2      -> Buzzer
3      -> Relay Toggle
4      -> Servo 0°
5      -> Servo 90°
6      -> Servo 180°
==========================================
*/

#include <IRremote.hpp>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define IR_PIN      3
#define SERVO_PIN   10
#define RGB_PIN     11
#define BUZZER_PIN  12
#define RELAY_PIN   A2

#define NUMPIXELS 8
Adafruit_NeoPixel rgb(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

Servo servo1;

bool relayState = false;

unsigned long lastCommandTime = 0;
const unsigned long DEBOUNCE_DELAY = 400;

void setup()
{
  Serial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  servo1.attach(SERVO_PIN);
  servo1.write(90);

  rgb.begin();
  rgb.clear();
  rgb.show();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("=== Project-27 ===");
  Serial.println("IR Remote Switch Ready");
}

void loop()
{
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
    {
      IrReceiver.resume();
      return;
    }

    if (millis() - lastCommandTime < DEBOUNCE_DELAY)
    {
      IrReceiver.resume();
      return;
    }

    uint8_t cmd = IrReceiver.decodedIRData.command;

    if (cmd == 0x0)
    {
      IrReceiver.resume();
      return;
    }

    lastCommandTime = millis();

    Serial.print("Command : 0x");
    Serial.println(cmd, HEX);

    switch (cmd)
    {
      case 0x09:
        allOff();
        Serial.println("All OFF");
        break;

      case 0x0D:
        setRGB(255,0,0);
        Serial.println("RED");
        break;

      case 0x19:
        beepBuzzer();
        Serial.println("Buzzer");
        break;

      case 0x1B:
        relayState = !relayState;

        if(relayState)
        {
          digitalWrite(RELAY_PIN,LOW);
          Serial.println("Relay ON");
        }
        else
        {
          digitalWrite(RELAY_PIN,HIGH);
          Serial.println("Relay OFF");
        }
        break;

      case 0x01:
        servo1.write(0);
        Serial.println("Servo 0");
        break;

      case 0x11:
        servo1.write(90);
        Serial.println("Servo 90");
        break;

      case 0x15:
        servo1.write(180);
        Serial.println("Servo 180");
        break;
    }

    IrReceiver.resume();
  }
}

void setRGB(byte r, byte g, byte b)
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    rgb.setPixelColor(i,rgb.Color(r,g,b));
  }
  rgb.show();
}

void beepBuzzer()
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

void allOff()
{
  rgb.clear();
  rgb.show();

  digitalWrite(RELAY_PIN,HIGH);

  digitalWrite(BUZZER_PIN, LOW);

  servo1.write(90);

  relayState=false;
}
