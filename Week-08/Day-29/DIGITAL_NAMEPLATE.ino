/*
====================================================
 Project-29 : Digital Nameplate (Custom Text + Typing Effect)
 Board : Young Engineer Board
 OLED  : SSD1306 128x64 I2C
====================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Custom Typing Effect Function
void typeText(String text, int x, int y, int size, int speedDelay)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(size);

  for (int i = 0; i < text.length(); i++)
  {
    display.setCursor(x, y);
    display.print(text.substring(0, i + 1));
    display.display();
    delay(speedDelay);
  }
}

void setup()
{
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("OLED Not Found! Check connections."));
    while (1);
  }

  Serial.println(F("================================="));
  Serial.println(F("   PROJECT-29: DIGITAL NAMEPLATE "));
  Serial.println(F("================================="));

  // 1. WELCOME SCREEN
  Serial.println(F("Screen: WELCOME"));
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 24);
  display.println(F("WELCOME"));
  display.display();
  delay(2000);

  // 2. TYPING EFFECT - Custom Line 1
  Serial.println(F("Typing: HAPPY INDEPENDENCE DAY"));
  typeText("INDEPENDENCE DAY", 10, 24, 1, 80);
  delay(1500);

  // 3. TYPING EFFECT - Custom Line 2
  Serial.println(F("Typing: LITTLE ENGINEER"));
  typeText("LITTLE ENGINEER", 18, 24, 1, 80);
  delay(1500);

  // 4. FINAL DISPLAY (Medium Text with Outer Border)
  Serial.println(F("Status: Showing Final Custom Nameplate"));
  display.clearDisplay();

  // Draw Outer Decorative Border
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextColor(SSD1306_WHITE);

  // You can change text size and message here!
  display.setTextSize(2);
  
  display.setCursor(18, 12);
  display.println(F("HAPPY"));

  display.setCursor(18, 36);
  display.println(F("INDIA"));

  display.display();
}

void loop()
{
  // Final display stays fixed on screen
}