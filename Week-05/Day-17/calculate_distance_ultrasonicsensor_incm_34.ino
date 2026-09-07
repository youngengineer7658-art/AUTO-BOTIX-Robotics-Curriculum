#include <Adafruit_NeoPixel.h>

#define TRIG 5
#define ECHO A3

#define LED_PIN 11
#define LED_COUNT 8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long duration;
float distance;

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);

  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {

  // Trigger Pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0) {

    Serial.println("Out of Range");

    strip.clear();
    strip.show();
  }

  else {

    distance = duration / 58.0;

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.print(" cm   ");

    if (distance <= 10) {

      Serial.println("VERY CLOSE");

      setColor(255,0,0);        // RED

    }

    else if (distance <= 20) {

      Serial.println("CLOSE");

      setColor(255,255,0);      // YELLOW

    }

    else if (distance <= 40) {

      Serial.println("MEDIUM");

      setColor(0,0,255);        // BLUE

    }

    else {

      Serial.println("FAR");

      setColor(0,255,0);        // GREEN

    }

  }

  delay(200);
}

void setColor(int r,int g,int b){

  for(int i=0;i<LED_COUNT;i++)
    strip.setPixelColor(i,strip.Color(r,g,b));

  strip.show();

}