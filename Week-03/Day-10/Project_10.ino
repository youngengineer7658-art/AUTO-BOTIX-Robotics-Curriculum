/*
=========================================================
   Day 10 | Practical-20: Motor Direction Reverser
   Features:
   - Alternates motor rotation (Clockwise & Anticlockwise)
   - Prints current rotation direction on Serial Monitor
=========================================================
*/

#define M1    2
#define M2    4
#define M_EN  6

void setup() {
  // Serial Communication Start (Baud Rate: 9600)
  Serial.begin(9600);

  // Pin Modes
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  // Enable Motor Driver Channel
  digitalWrite(M_EN, HIGH);

  Serial.println(F("===================================="));
  Serial.println(F(" Practical-20: Motor Direction Test "));
  Serial.println(F("===================================="));
  delay(1000);
}

void loop() {
  // 1. Clockwise Direction (Forward)
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  Serial.println(F("[MOTOR STATUS] Rotating: CLOCKWISE (Forward)"));
  delay(3000);

  // 2. Short Pause / Brake (Safety for Driver)
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  Serial.println(F("[MOTOR STATUS] Stopped"));
  delay(500);

  // 3. Anticlockwise Direction (Reverse)
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  Serial.println(F("[MOTOR STATUS] Rotating: ANTICLOCKWISE (Reverse)"));
  delay(3000);

  // 4. Short Pause / Brake
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  Serial.println(F("[MOTOR STATUS] Stopped"));
  delay(500);
}
