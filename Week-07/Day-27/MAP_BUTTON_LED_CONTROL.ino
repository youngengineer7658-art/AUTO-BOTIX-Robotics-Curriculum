/*
==================================================
Practical-54 : IR → RGB LEDs
Autobotix Robotics Board

TSOP      -> D3
WS2812B   -> D11
RGB LEDs  -> 8
==================================================
*/

#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>

#define IR_PIN      3
#define RGB_PIN     11
#define NUMPIXELS   8

Adafruit_NeoPixel rgb(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

//--------------- IR Commands ----------------

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

#define OK      0x05

#define UP      0x0E
#define DOWN    0x1A
#define LEFT    0x0A
#define RIGHT   0x1E

//--------------------------------------------

int brightness = 80;

uint8_t colorIndex = 0;

uint32_t colors[] =
{
  rgb.Color(255,0,0),      //Red
  rgb.Color(0,255,0),      //Green
  rgb.Color(0,0,255),      //Blue
  rgb.Color(255,255,0),    //Yellow
  rgb.Color(0,255,255),    //Cyan
  rgb.Color(255,0,255),    //Magenta
  rgb.Color(255,255,255)   //White
};

//--------------------------------------------

void showColor(uint32_t c)
{
  rgb.setBrightness(brightness);

  for(int i=0;i<NUMPIXELS;i++)
  {
    rgb.setPixelColor(i,c);
  }

  rgb.show();
}

//--------------------------------------------

void rgbOff()
{
  rgb.clear();
  rgb.show();
}

//--------------------------------------------

void rainbow()
{
  for(long firstPixelHue=0; firstPixelHue<65536; firstPixelHue+=256)
  {
    for(int i=0;i<NUMPIXELS;i++)
    {
      int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);

      rgb.setPixelColor(i,
      rgb.gamma32(rgb.ColorHSV(pixelHue)));
    }

    rgb.setBrightness(brightness);
    rgb.show();

    delay(15);

    if(IrReceiver.decode())
    {
      IrReceiver.resume();
      return;
    }
  }
}

//--------------------------------------------

void policeFlash()
{
  for(int j=0;j<10;j++)
  {
    for(int i=0;i<NUMPIXELS;i++)
      rgb.setPixelColor(i,rgb.Color(255,0,0));

    rgb.show();

    delay(120);

    for(int i=0;i<NUMPIXELS;i++)
      rgb.setPixelColor(i,rgb.Color(0,0,255));

    rgb.show();

    delay(120);
  }

  rgbOff();
}

//--------------------------------------------

void setup()
{
  Serial.begin(9600);

  rgb.begin();
  rgb.show();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Autobotix Practical-54");
}

//--------------------------------------------

void loop()
{
  if(IrReceiver.decode())
  {
    uint8_t cmd = IrReceiver.decodedIRData.command;

    Serial.print("Command : 0x");
    Serial.println(cmd,HEX);

    switch(cmd)
    {
      case POWER:
      case BTN0:
        rgbOff();
      break;

      case BTN1:
        colorIndex=0;
        showColor(colors[colorIndex]);
      break;

      case BTN2:
        colorIndex=1;
        showColor(colors[colorIndex]);
      break;

      case BTN3:
        colorIndex=2;
        showColor(colors[colorIndex]);
      break;

      case BTN4:
        colorIndex=3;
        showColor(colors[colorIndex]);
      break;

      case BTN5:
        colorIndex=4;
        showColor(colors[colorIndex]);
      break;

      case BTN6:
        colorIndex=5;
        showColor(colors[colorIndex]);
      break;

      case BTN7:
        colorIndex=6;
        showColor(colors[colorIndex]);
      break;

      case BTN8:
        rainbow();
      break;

      case BTN9:
        policeFlash();
      break;

      case OK:
        colorIndex++;

        if(colorIndex>6)
          colorIndex=0;

        showColor(colors[colorIndex]);
      break;

      case UP:

        brightness +=20;

        if(brightness>255)
          brightness=255;

        showColor(colors[colorIndex]);

      break;

      case DOWN:

        brightness -=20;

        if(brightness<10)
          brightness=10;

        showColor(colors[colorIndex]);

      break;

      case RIGHT:

        colorIndex++;

        if(colorIndex>6)
          colorIndex=0;

        showColor(colors[colorIndex]);

      break;

      case LEFT:

        if(colorIndex==0)
          colorIndex=6;
        else
          colorIndex--;

        showColor(colors[colorIndex]);

      break;
    }

    IrReceiver.resume();
  }
}