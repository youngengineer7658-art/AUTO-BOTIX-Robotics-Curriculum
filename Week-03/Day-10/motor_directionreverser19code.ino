#define M1    2
#define M2    4
#define M_EN  6

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  digitalWrite(M_EN, HIGH);

  // Reverse Rotation (M1 = LOW, M2 = HIGH)
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);

  Serial.println("Motor Driver Enabled");
  Serial.println("Motor Direction: REVERSE");
}

void loop() {
  // Motor continuously spinning in reverse
}