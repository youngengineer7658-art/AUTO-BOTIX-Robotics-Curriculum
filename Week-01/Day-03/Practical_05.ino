#define BUTTON_PIN A6

String lastButton = "NONE";
String candidateButton = "NONE";
unsigned long candidateStartTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

void setup() {
  Serial.begin(9600);
}

String readButton(int value) {
  if (value < 300) {
    return "NONE";       // idle state (0)
  }
  else if (value < 490) {
    return "UP";         // ~420
  }
  else if (value < 660) {
    return "DOWN";       // ~560
  }
  else if (value < 800) {
    return "BACK";       // ~765
  }
  else if (value < 930) {
    return "SELECT";     // ~843
  }
  else {
    return "NONE";
  }
}

void loop() {
  int value = analogRead(BUTTON_PIN);
  String reading = readButton(value);

  if (reading != candidateButton) {
    candidateButton = reading;
    candidateStartTime = millis();
  }

  if ((millis() - candidateStartTime) > DEBOUNCE_DELAY) {
    if (candidateButton != lastButton) {
      if (candidateButton != "NONE") {
        Serial.println(candidateButton);
      }
      lastButton = candidateButton;
    }
  }

  delay(10);
}
