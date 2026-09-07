// AUTO-BOTIX Robot Car
// Project : Wired Robot Car (Manual Control)

#define M11  2
#define M12  4
#define MFN  6      // Motor 1 Enable
#define M2EN 9      // Motor 2 Enable
#define M21  7
#define M22  8
#define BTN  A6

// Calibrated Ranges (Aapke Board ka Exact Data)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);
  pinMode(M2EN, OUTPUT);

  digitalWrite(MFN, HIGH);
  digitalWrite(M2EN, HIGH);

  Serial.println("===== WIRED ROBOT CAR STARTED =====");
}

void moveForward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void moveBackward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
}

void leftTurn()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void rightTurn()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void loop()
{
  int value = analogRead(BTN);

  // UP (853 - 855) -> FORWARD
  if (value >= BTN_UP_MIN && value <= BTN_UP_MAX) {
    moveForward();
    Serial.println("FORWARD");
  }

  // DOWN (789 - 790) -> BACKWARD
  else if (value >= BTN_DOWN_MIN && value <= BTN_DOWN_MAX) {
    moveBackward();
    Serial.println("BACKWARD");
  }

  // BACK (733 - 734) -> LEFT TURN
  else if (value >= BTN_BACK_MIN && value <= BTN_BACK_MAX) {
    leftTurn();
    Serial.println("LEFT TURN");
  }

  // SELECT (931 - 932) -> RIGHT TURN
  else if (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX) {
    rightTurn();
    Serial.println("RIGHT TURN");
  }

  // No button -> STOP
  else {
    stopMotor();
  }

  delay(100);
}
