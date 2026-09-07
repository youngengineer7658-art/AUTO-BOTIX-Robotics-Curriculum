// Practical-17 : Spin Motor Forward

#define M1    2
#define M2    4
#define M_EN  6

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  // Enable Motor Driver
  digitalWrite(M_EN, HIGH);

  // Motor Forward
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);

  Serial.println("Motor Driver Enabled");
  Serial.println("Motor Direction: FORWARD");
}

void loop() {
  // Motor continuously forward
}
