#define M1      2
#define M2      4
#define M21     7
#define M22     8
#define M_EN    6      // Motor 1 Enable / PWM
#define M2_EN   9      // Motor 2 Enable / PWM
#define BTN     A6

bool motorsOn = false;
bool buttonPressed = false;

void setup() {
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(M_EN, OUTPUT);
  pinMode(M2_EN, OUTPUT);

  Serial.begin(9600);
  Serial.println("===== CONTROL 2 MOTORS WITH BUTTON STARTED =====");
}

void loop() {

  int button = analogRead(BTN);

  // Button Released (idle value ~0)
  if (button < 200) {
    buttonPressed = false;
  }

  // SELECT (931 - 932) -> Naya button press detect (sirf ek baar trigger)
  else if (button >= 920 && button <= 945 && !buttonPressed) {

    motorsOn = !motorsOn;   // toggle karo

    if (motorsOn) {
      digitalWrite(M_EN, HIGH);
      digitalWrite(M2_EN, HIGH);

      // Motor1 Forward
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);

      // Motor2 Forward
      digitalWrite(M21, HIGH);
      digitalWrite(M22, LOW);

      Serial.println("BOTH MOTORS ON");
    }
    else {
      digitalWrite(M_EN, LOW);
      digitalWrite(M2_EN, LOW);

      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      digitalWrite(M21, LOW);
      digitalWrite(M22, LOW);

      Serial.println("BOTH MOTORS OFF");
    }

    buttonPressed = true;
  }

  delay(20);
}
