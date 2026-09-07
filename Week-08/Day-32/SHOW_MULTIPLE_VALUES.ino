/*
==========================================
Practical-63 : Multiple Sensor Values
OLED + Ultrasonic + POT + Button
==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TRIG_PIN 5
#define ECHO_PIN A3

#define POT_PIN A7
#define BUTTON_PIN A6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int lastDistance = -999;
int lastPot = -999;
int lastBtn = -999;

const int DIST_THRESHOLD = 2;
const int POT_THRESHOLD = 10;

//---------------------------------
int getButton()
//---------------------------------
{
  long sum = 0;

  for(int i = 0; i < 5; i++)
  {
    sum += analogRead(BUTTON_PIN);
    delay(2);
  }

  int value = sum / 5;

  // UP (853 - 855)
  if(value >= 845 && value <= 865)
    return 1;

  // DOWN (789 - 790)
  if(value >= 780 && value <= 800)
    return 2;

  // BACK (733 - 734)
  if(value >= 725 && value <= 745)
    return 3;

  // SELECT (931 - 932)
  if(value >= 920 && value <= 945)
    return 4;

  return 0;
}

//---------------------------------
String buttonName(int btn)
//---------------------------------
{
  switch(btn)
  {
    case 1: return "UP";
    case 2: return "DOWN";
    case 3: return "BACK";
    case 4: return "SELECT";
    default: return "NONE";
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C))
  {
    while(1);
  }

  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  // Ultrasonic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  int distance;

  if(duration == 0)
    distance = -1;
  else
    distance = duration * 0.034 / 2;

  // Pot
  int pot = analogRead(POT_PIN);

  // Button
  int btn = getButton();

  // Sirf tab update karo jab koi value meaningfully change ho
  bool distChanged = abs(distance - lastDistance) > DIST_THRESHOLD;
  bool potChanged = abs(pot - lastPot) > POT_THRESHOLD;
  bool btnChanged = btn != lastBtn;

  if (distChanged || potChanged || btnChanged)
  {
    // Serial Monitor
    Serial.print("Distance : ");
    Serial.print(distance);

    Serial.print(" cm   POT : ");
    Serial.print(pot);

    Serial.print("   Button : ");
    Serial.println(buttonName(btn));

    // OLED
    display.clearDisplay();

    display.drawRect(0,0,128,64,SSD1306_WHITE);

    display.setTextSize(1);

    display.setCursor(12,3);
    display.println("SENSOR MONITOR");

    display.drawLine(5,14,122,14,SSD1306_WHITE);

    display.setCursor(5,20);
    display.print("DIST : ");

    if(distance == -1)
      display.print("--");
    else
      display.print(distance);

    display.print(" cm");

    display.setCursor(5,35);
    display.print("POT  : ");
    display.print(pot);

    display.setCursor(5,50);
    display.print("BTN  : ");
    display.print(buttonName(btn));

    display.display();

    lastDistance = distance;
    lastPot = pot;
    lastBtn = btn;
  }

  delay(50);
}