// Practical : Left Right Turn Control

#define M1      2
#define M2      4
#define M21     7
#define M22     8
#define M_EN    6      // Motor 1 Enable
#define M2_EN   9      // Motor 2 Enable
#define BTN     A6

// Calibrated Ranges
#define BTN_UP_MIN   845
#define BTN_UP_MAX   865
#define BTN_DOWN_MIN 780
#define BTN_DOWN_MAX 800

void setup() {
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(M_EN, OUTPUT);
  pinMode(M2_EN, OUTPUT);

  analogWrite(M_EN, 255);
  analogWrite(M2_EN, 255);

  Serial.begin(9600);
  Serial.println("===== LEFT RIGHT TURN CONTROL STARTED =====");
}

void loop() {

  int value = analogRead(BTN);

  // UP (853 - 855) -> LEFT TURN (Motor 2 forward, Motor 1 stop)
  if (value >= BTN_UP_MIN && value <= BTN_UP_MAX) {

    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);

    digitalWrite(M21, HIGH);
    digitalWrite(M22, LOW);

    Serial.println("LEFT TURN");
  }

  // DOWN (789 - 790) -> RIGHT TURN (Motor 1 forward, Motor 2 stop)
  else if (value >= BTN_DOWN_MIN && value <= BTN_DOWN_MAX) {

    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);

    digitalWrite(M21, LOW);
    digitalWrite(M22, LOW);

    Serial.println("RIGHT TURN");
  }

  // No button -> STOP
  else {

    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);
    digitalWrite(M21, LOW);
    digitalWrite(M22, LOW);
  }

  delay(100);
}