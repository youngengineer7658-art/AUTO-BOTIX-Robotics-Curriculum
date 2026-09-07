/*
=========================================================
   Day 36 | Practical-72 : Combine Code Modules (NO BEEP FIX)
=========================================================
*/

#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

//================ OLED DISPLAY ===================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool oledReady = false;

//================ RGB NEOPIXEL ====================
#define RGB_PIN 11
#define NUMPIXELS 8
Adafruit_NeoPixel rgb(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

//================ SERVO MOTOR ====================
Servo scanServo;

//================ MOTOR DRIVER PINS ==============
#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

//================ SENSORS =======================
#define TRIG_PIN 5
#define ECHO_PIN A3
#define IR_L A0
#define IR_R A1

//================ PERIPHERALS ===================
#define BUZZER_PIN 12

int frontDistance = 100;
String lastStatus = "";

void setRGB(byte r, byte g, byte b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    rgb.setPixelColor(i, rgb.Color(r, g, b));
  }
  rgb.show();
}

void updateDisplay(String statusText) {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 2);
  display.println("PRACTICAL 72");
  display.drawLine(0, 14, 127, 14, SSD1306_WHITE);

  display.setCursor(0, 24);
  display.print("Dist : ");
  if (frontDistance >= 400 || frontDistance <= 0) display.println("CLEAR");
  else { display.print(frontDistance); display.println(" cm"); }

  display.setCursor(0, 44);
  display.print("Status: ");
  display.println(statusText);
  display.display();
}

void processStatus(String status) {
  if (status != lastStatus) {
    updateDisplay(status);
    Serial.print("Status: "); Serial.print(status);
    Serial.print(" | Dist: "); Serial.println(frontDistance);
    lastStatus = status;
  }
}

//================ MOTOR FUNCTIONS =================
void stopCar() {
  digitalWrite(M11, LOW);  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);  digitalWrite(M22, LOW);
  digitalWrite(MFN, LOW);
}

void forward() {
  digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
  digitalWrite(MFN, HIGH);
}

void backward() {
  digitalWrite(M11, LOW);  digitalWrite(M12, HIGH);
  digitalWrite(M21, LOW);  digitalWrite(M22, HIGH);
  digitalWrite(MFN, HIGH);
}

void leftTurn() {
  digitalWrite(M11, LOW);  digitalWrite(M12, HIGH);
  digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
  digitalWrite(MFN, HIGH);
}

void rightTurn() {
  digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);  digitalWrite(M22, HIGH);
  digitalWrite(MFN, HIGH);
}

// Strict Distance Reader (Ignores 0 and junk values)
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 15000); 

  if (duration == 0) return 400; // 0 noise ko direct 400 (Clear) bana do
  
  int cm = duration * 0.034 / 2;
  if (cm <= 2) return 400; // Fake 0-2cm zero reading filter
  return cm;
}

void setup() {
  Serial.begin(9600);

  // Pins Setup
  pinMode(M11, OUTPUT); pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT); pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);
  stopCar();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_L, INPUT);
  pinMode(IR_R, INPUT);
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Force Buzzer OFF initially

  // Single short beep at start
  digitalWrite(BUZZER_PIN, HIGH); delay(80); digitalWrite(BUZZER_PIN, LOW);

  // Servo Setup
  scanServo.attach(10);
  scanServo.write(90);

  // RGB Setup
  rgb.begin();
  setRGB(0, 255, 0); // Green

  // OLED Setup
  Wire.begin();
  Wire.setClock(100000);
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C) || display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    oledReady = true;
  }

  processStatus("SYSTEM READY");
  delay(300);
}

void loop() {
  frontDistance = getDistance();
  int leftIR = digitalRead(IR_L);
  int rightIR = digitalRead(IR_R);

  // 1. REAL FRONT OBSTACLE (Strictly between 3cm and 20cm)
  if (frontDistance >= 3 && frontDistance <= 20) {
    stopCar();
    processStatus("OBSTACLE!");
    setRGB(255, 0, 0); // Red Light

    // Only beep ONCE when valid object is detected
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);

    backward(); delay(200); stopCar();

    // Servo Scan
    scanServo.write(30); delay(250);
    int leftDist = getDistance();
    scanServo.write(150); delay(300);
    int rightDist = getDistance();
    scanServo.write(90); delay(150);

    if (leftDist > rightDist) {
      leftTurn(); delay(300); stopCar();
    } else {
      rightTurn(); delay(300); stopCar();
    }
  }
  // 2. LEFT IR OBSTACLE
  else if (leftIR == LOW) {
    digitalWrite(BUZZER_PIN, LOW);
    processStatus("AVOID LEFT");
    setRGB(0, 0, 255); // Blue
    rightTurn(); delay(150);
  }
  // 3. RIGHT IR OBSTACLE
  else if (rightIR == LOW) {
    digitalWrite(BUZZER_PIN, LOW);
    processStatus("AVOID RIGHT");
    setRGB(0, 0, 255); // Blue
    leftTurn(); delay(150);
  }
  // 4. CLEAR PATH
  else {
    digitalWrite(BUZZER_PIN, LOW); // Force Silent
    processStatus("FORWARD");
    setRGB(0, 255, 0); // Green
    forward();
  }

  delay(30);
}