#include <Adafruit_NeoPixel.h>

#define LED_PIN   11
#define NUM_LEDS  8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setColor(byte r, byte g, byte b)
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
  strip.show();
}

void setup()
{
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(80);
  strip.clear();
  strip.show();

  Serial.println("===== Bluetooth RGB Control =====");
  Serial.println("R,G,B,W,Y,C,P,M,O,L,S,A,F,X");
}

void loop()
{
  if(Serial.available())
  {
    char cmd = toupper(Serial.read());

    Serial.print("Received : ");
    Serial.println(cmd);

    switch(cmd)
    {
      case 'R': setColor(255,0,0); break;         // Red
      case 'G': setColor(0,255,0); break;         // Green
      case 'B': setColor(0,0,255); break;         // Blue
      case 'W': setColor(255,255,255); break;     // White
      case 'Y': setColor(255,255,0); break;       // Yellow
      case 'C': setColor(0,255,255); break;       // Cyan
      case 'P': setColor(128,0,255); break;       // Purple
      case 'M': setColor(255,0,255); break;       // Magenta/Pink
      case 'O': setColor(255,80,0); break;        // Orange
      case 'L': setColor(100,255,0); break;       // Lime
      case 'S': setColor(0,150,255); break;       // Sky Blue
      case 'A': setColor(0,255,180); break;       // Aqua
      case 'F': setColor(255,40,0); break;        // Fire Orange
      case 'X':
        strip.clear();
        strip.show();
        break;                                    // OFF

      default:
        Serial.println("Invalid Command");
        break;
    }
  }
}