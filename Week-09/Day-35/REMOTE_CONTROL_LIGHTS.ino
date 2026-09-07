/*
==================================================
Practical-69 : Remote Controlled Lights
==================================================

Objective:
To control the onboard WS2812B RGB LEDs wirelessly
using an IR Remote.

Hardware Required:
• Autobotix Robotics Board
• IR Remote
• Onboard TSOP IR Receiver
• Onboard WS2812B RGB LEDs

Pin Mapping:

TSOP Receiver  -> D3
WS2812B RGB    -> D11

Button Mapping:

POWER -> RGB OFF

1 -> RED
2 -> GREEN
3 -> BLUE
4 -> YELLOW
5 -> CYAN
6 -> MAGENTA
7 -> WHITE
8 -> Rainbow Effect
9 -> Police Flash
0 -> RGB OFF

==================================================
*/

#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

#define IR_PIN      3
#define RGB_PIN     11
#define NUM_LEDS    8

Adafruit_NeoPixel rgb(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

//------------- Remote Commands -------------

#define POWER   0x09

#define BTN1    0x0D
#define BTN2    0x19
#define BTN3    0x1B
#define BTN4    0x01
#define BTN5    0x11
#define BTN6    0x15
#define BTN7    0x17
#define BTN8    0x06
#define BTN9    0x12
#define BTN0    0x16

//-------------------------------------------

void showColor(uint8_t r,uint8_t g,uint8_t b)
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    rgb.setPixelColor(i,rgb.Color(r,g,b));
  }

  rgb.show();
}

void rgbOff()
{
  rgb.clear();
  rgb.show();
}

void rainbow()
{
  for(int j=0;j<256;j++)
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      rgb.setPixelColor(i,
      rgb.gamma32(rgb.ColorHSV((i*65536L/NUM_LEDS)+(j*256))));
    }

    rgb.show();
    delay(10);

    if(IrReceiver.decode())
    {
      IrReceiver.resume();
      return;
    }
  }
}

void policeFlash()
{
  for(int k=0;k<10;k++)
  {
    showColor(255,0,0);
    delay(150);

    showColor(0,0,255);
    delay(150);
  }

  rgbOff();
}

void setup()
{
  Serial.begin(9600);

  rgb.begin();
  rgb.clear();
  rgb.show();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Practical-69 : Remote Controlled Lights");
}

void loop()
{
  if(IrReceiver.decode())
  {
    byte cmd = IrReceiver.decodedIRData.command;

    switch(cmd)
    {
      case POWER:
      case BTN0:
        rgbOff();
        Serial.println("RGB OFF");
      break;

      case BTN1:
        showColor(255,0,0);
        Serial.println("RED");
      break;

      case BTN2:
        showColor(0,255,0);
        Serial.println("GREEN");
      break;

      case BTN3:
        showColor(0,0,255);
        Serial.println("BLUE");
      break;

      case BTN4:
        showColor(255,255,0);
        Serial.println("YELLOW");
      break;

      case BTN5:
        showColor(0,255,255);
        Serial.println("CYAN");
      break;

      case BTN6:
        showColor(255,0,255);
        Serial.println("MAGENTA");
      break;

      case BTN7:
        showColor(255,255,255);
        Serial.println("WHITE");
      break;

      case BTN8:
        Serial.println("RAINBOW");
        rainbow();
      break;

      case BTN9:
        Serial.println("POLICE FLASH");
        policeFlash();
      break;
    }

    IrReceiver.resume();
  }
}