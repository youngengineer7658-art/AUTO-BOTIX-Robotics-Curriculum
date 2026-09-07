// Project: Electric Fan (DC Motor with Blade) ON/OFF with Button

#define M1    2
#define M2    4
#define M_EN  6
#define BTN   A6

bool fanState = false;
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  Serial.println("===== ELECTRIC FAN PROJECT STARTED =====");
}

void loop() {

  int button = analogRead(BTN);

  // Button Released (idle value ~0)
  if (button < 300) {
    buttonPressed = false;
  }

  // Naya button press detect (sirf ek baar trigger)
  else if (button > 300 && !buttonPressed) {

    fanState = !fanState;   // toggle karo

    if (fanState) {
      digitalWrite(M_EN, HIGH);
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      Serial.println("FAN ON");
    }
    else {
      digitalWrite(M_EN, LOW);
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      Serial.println("FAN OFF");
    }

    buttonPressed = true;
  }

  delay(20);
}
