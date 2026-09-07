/*
==========================================
Practical-61 : Button -> Text Change
Auto-Botix Board
Button : A6
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_PIN A6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String lastButton = "";

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
  int value = analogRead(BUTTON_PIN);

  String button = "NO BUTTON";

  // UP (853 - 855)
  if (value >= 845 && value <= 865)
  {
    button = "UP";
  }
  // DOWN (789 - 790)
  else if (value >= 780 && value <= 800)
  {
    button = "DOWN";
  }
  // BACK (733 - 734)
  else if (value >= 725 && value <= 745)
  {
    button = "BACK";
  }
  // SELECT (931 - 932)
  else if (value >= 920 && value <= 945)
  {
    button = "SELECT";
  }

  // Sirf tab update karo jab button state change ho
  if (button != lastButton)
  {
    Serial.print("Value : ");
    Serial.print(value);
    Serial.print("   Button : ");
    Serial.println(button);

    display.clearDisplay();

    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(22, 8);
    display.println("BUTTON STATUS");

    display.drawLine(10, 20, 118, 20, SSD1306_WHITE);

    display.setTextSize(2);

    if (button == "UP")
      display.setCursor(42, 36);
    else if (button == "DOWN")
      display.setCursor(28, 36);
    else if (button == "BACK")
      display.setCursor(28, 36);
    else if (button == "SELECT")
      display.setCursor(16, 36);
    else
      display.setCursor(8, 36);

    display.println(button);

    display.display();

    lastButton = button;
  }

  delay(50);
}