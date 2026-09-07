/*
==========================================
Practical-58 : Change Text Size
OLED : SSD1306 128x64 I2C + Serial Monitor
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
  // Serial Monitor Communication Start (9600 Baud)
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Not Found! Check connections.");
    while (1);
  }

  Serial.println("=================================");
  Serial.println("    OLED Text Size Demo Ready    ");
  Serial.println("=================================");
  delay(1000);
}

void loop()
{
  //================ SMALL =================
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Small Text");

  display.display();

  // Serial Monitor Status Output
  Serial.println("[OLED STATUS] Current Size: SMALL (Size 1)");

  delay(2000);

  //================ MEDIUM ================
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 15);
  display.println("Medium");

  display.display();

  // Serial Monitor Status Output
  Serial.println("[OLED STATUS] Current Size: MEDIUM (Size 2)");

  delay(2000);

  //================ LARGE =================
  display.clearDisplay();

  display.setTextSize(3);
  display.setCursor(0, 18);
  display.println("Large");

  display.display();

  // Serial Monitor Status Output
  Serial.println("[OLED STATUS] Current Size: LARGE (Size 3)");

  delay(2000);
}