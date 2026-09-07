// Practical : Motor ON/OFF with Button

#define M1    2
#define M2    4
#define M_EN  6
#define BTN   A6

bool motorState = false;     // false = OFF, true = ON
bool buttonPressed = false;  // edge detection ke liye

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  Serial.println("===== MOTOR ON/OFF STARTED =====");
}

void loop() {

  int button = analogRead(BTN);

  // Button Released (idle value ~0)
  if (button < 300) {
    buttonPressed = false;
  }

  // Naya button press detect (sirf ek baar trigger)
  else if (button > 300 && !buttonPressed) {

    motorState = !motorState;   // toggle karo

    if (motorState) {
      digitalWrite(M_EN, HIGH);
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      Serial.println("MOTOR ON");
    }
    else {
      digitalWrite(M_EN, LOW);
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      Serial.println("MOTOR OFF");
    }

    buttonPressed = true;
  }

  delay(20);
}