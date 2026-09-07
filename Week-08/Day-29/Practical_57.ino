/*
==========================================
Practical-57 : Print Text on OLED
OLED : SSD1306 128x64 I2C
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Not Found");
    while(1);
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,10);
  display.println("HELLO");

  display.setTextSize(1);
  display.setCursor(10,40);
  display.println("WELCOME TO OLED");

  display.display();

  Serial.println("OLED Working");
}

void loop()
{
}
