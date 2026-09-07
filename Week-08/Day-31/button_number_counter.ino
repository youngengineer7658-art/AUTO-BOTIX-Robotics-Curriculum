/*
==========================================
Practical-62 : 0–9 Button Counter
==========================================

UP      = 845–865 (Actual: 853-855)
DOWN    = 780–800 (Actual: 789-790)
BACK    = 725–745 (Actual: 733-734)
SELECT  = 920–945 (Actual: 931-932)

OLED  : A4(SDA), A5(SCL)
Button: A6
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_PIN A6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int counter = 0;
int lastButton = 0;

//==========================
int getButton()
//==========================
{
  long sum = 0;

  for (int i = 0; i < 5; i++)
  {
    sum += analogRead(BUTTON_PIN);
    delay(2);
  }

  int value = sum / 5;

  // UP (853 - 855)
  if (value >= 845 && value <= 865)
    return 1;      // UP

  // DOWN (789 - 790)
  if (value >= 780 && value <= 800)
    return 2;      // DOWN

  // BACK (733 - 734)
  if (value >= 725 && value <= 745)
    return 3;      // BACK

  // SELECT (931 - 932)
  if (value >= 920 && value <= 945)
    return 4;      // SELECT

  return 0;
}

void setup()
{
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  int button = getButton();

  if (button != 0 && lastButton == 0)
  {
    switch (button)
    {
      case 1:   // UP -> Increment
        counter++;
        if (counter > 9)
          counter = 0;
        break;

      case 2:   // DOWN -> Decrement
        counter--;
        if (counter < 0)
          counter = 9;
        break;

      case 3:   // BACK
        // No Action
        break;

      case 4:   // SELECT -> Reset
        counter = 0;
        break;
    }
  }

  lastButton = button;

  Serial.print("Counter : ");
  Serial.println(counter);

  display.clearDisplay();

  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(22, 6);
  display.println("DIGITAL COUNTER");

  display.drawLine(8, 18, 120, 18, SSD1306_WHITE);

  display.setTextSize(4);
  display.setCursor(50, 28);
  display.print(counter);

  display.display();

  delay(30);
}