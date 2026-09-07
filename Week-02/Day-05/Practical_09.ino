#define POT_PIN A7

int lastValue = -1;
const int THRESHOLD = 5;   // itni jitter ignore hogi

void setup() {
  Serial.begin(9600);
}

void loop() {

  int potValue = analogRead(POT_PIN);

  // Sirf tab print karo jab value THRESHOLD se zyada change ho
  if (abs(potValue - lastValue) > THRESHOLD) {

    Serial.print("Potentiometer Value = ");
    Serial.println(potValue);

    lastValue = potValue;
  }

  delay(50);
}
