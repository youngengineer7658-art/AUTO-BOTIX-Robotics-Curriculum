// Project : Speed Controlled Fan

#define M1      2
#define M2      4
#define M_EN    6      // PWM pin
#define POT_PIN A7

int lastSpeed = -1;
const int THRESHOLD = 5;

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  // Fan direction fix (Forward)
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);

  Serial.println("===== SPEED CONTROLLED FAN STARTED =====");
}

void loop() {

  int potValue = analogRead(POT_PIN);

  // Potentiometer value (0-1023) ko PWM range (0-255) mein convert karo
  int speed = map(potValue, 0, 1023, 0, 255);

  // Fan speed apply karo
  analogWrite(M_EN, speed);

  // Sirf tab print karo jab speed meaningfully change ho
  if (abs(speed - lastSpeed) > THRESHOLD) {
    Serial.print("Pot Raw: ");
    Serial.print(potValue);
    Serial.print(" | Fan Speed (PWM): ");
    Serial.println(speed);

    lastSpeed = speed;
  }

  delay(10);
}
