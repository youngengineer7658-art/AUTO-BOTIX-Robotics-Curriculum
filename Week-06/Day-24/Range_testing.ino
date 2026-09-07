/*
Practical-47 : Bluetooth Range Testing
Har second ek "ping" bhejta/receive karta hai connection verify karne ke liye
*/

unsigned long lastReceived = 0;
unsigned long lastPing = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("===== RANGE TEST STARTED =====");
  Serial.println("Move away from phone and press any button to test range");
}

void loop() {

  if (Serial.available()) {
    char c = Serial.read();
    lastReceived = millis();

    Serial.print("Signal OK - Received: ");
    Serial.println(c);
  }

  // Agar 3 second se koi signal nahi aaya, warn karo
  if (millis() - lastPing > 3000) {
    if (millis() - lastReceived > 3000 && lastReceived != 0) {
      Serial.println("WARNING: No signal - possibly out of range!");
    }
    lastPing = millis();
  }
}