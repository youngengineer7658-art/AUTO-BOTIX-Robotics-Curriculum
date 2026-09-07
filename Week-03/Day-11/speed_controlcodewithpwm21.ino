// Practical : Motor Speed Control with PWM

#define M1     2
#define M2     4
#define M_EN   6      // PWM capable pin (Nano par D6 PWM support karta hai)
#define POT_PIN A7

int lastSpeed = -1;
const int THRESHOLD = 5;

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  // Motor direction fix kar do (Forward)
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);

  Serial.println("===== MOTOR SPEED CONTROL (PWM) STARTED =====");
}

void loop() {

  int potValue = analogRead(POT_PIN);

  // 0-1023 ko PWM range 0-255 mein convert karo
  int speed = map(potValue, 0, 1023, 0, 255);

  // PWM speed apply karo enable pin par
  analogWrite(M_EN, speed);

  // Sirf tab print karo jab speed meaningfully change ho
  if (abs(speed - lastSpeed) > THRESHOLD) {
    Serial.print("Pot Raw: ");
    Serial.print(potValue);
    Serial.print(" | Motor Speed (PWM): ");
    Serial.println(speed);

    lastSpeed = speed;
  }

  delay(10);
}