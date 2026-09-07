/*
==========================================
Practical-64 : OLED Display Animation
OLED : SSD1306 128x64
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//============================
// Loading Bar
//============================
void loadingBar()
{
  Serial.println("Animation : Loading Bar");

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,10);
  display.println("LOADING...");

  display.drawRect(10,35,108,12,SSD1306_WHITE);

  for(int i=0;i<=104;i+=4)
  {
    display.fillRect(12,37,i,8,SSD1306_WHITE);
    display.display();
    delay(60);
  }

  delay(1000);
}

//============================
// Typing Effect
//============================
void typingEffect(String text)
{
  Serial.print("Animation : Typing Effect - ");
  Serial.println(text);

  display.clearDisplay();

  display.setTextSize(2);

  for(int i=0;i<text.length();i++)
  {
    display.clearDisplay();

    display.setCursor(10,25);
    display.print(text.substring(0,i+1));

    display.display();

    delay(150);
  }

  delay(1000);
}

//============================
// Smooth Heart
//============================
void drawHeart(int s)
{
  display.fillCircle(64-s,24,s,SSD1306_WHITE);
  display.fillCircle(64+s,24,s,SSD1306_WHITE);

  display.fillTriangle(
    64-(2*s),26,
    64+(2*s),26,
    64,26+(3*s),
    SSD1306_WHITE);
}

void heartAnimation()
{
  Serial.println("Animation : Heartbeat");

  int beat[]={6,7,8,9,10,9,8,7,6};

  for(int i=0;i<9;i++)
  {
    display.clearDisplay();

    drawHeart(beat[i]);

    display.display();

    delay(70);
  }

  delay(300);
}

//============================
// Scrolling Text
//============================
void scrollText()
{
  Serial.println("Animation : Scrolling Text - WELCOME");

  for(int x=128;x>-180;x--)
  {
    display.clearDisplay();

    display.setTextSize(2);

    display.setCursor(x,25);
    display.print("WELCOME");

    display.display();

    delay(20);
  }
}

//============================
void setup()
//============================
{
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C))
  {
    Serial.println("OLED Failed");
    while(1);
  }

  display.setTextColor(SSD1306_WHITE);

  Serial.println("===== OLED DISPLAY ANIMATION STARTED =====");
}

//============================
void loop()
//============================
{
  loadingBar();

  typingEffect("HELLO");

  heartAnimation();

  scrollText();

  Serial.println("----- Cycle Complete -----");
  Serial.println();
}