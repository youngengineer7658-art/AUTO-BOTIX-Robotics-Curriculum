/*
==========================================
Practical-60 : OLED Scrolling Text
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

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  for (int x = 128; x > -180; x--)
  {
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(x, 25);
    display.print("WELCOME");

    display.display();

    delay(20);
  }
}