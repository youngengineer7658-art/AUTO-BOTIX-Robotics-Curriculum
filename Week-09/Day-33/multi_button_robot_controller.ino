/*
==========================================
Project-33 : Multi-Button Robot Controller
Autobotix Robotics Board

M11 -> D2
M12 -> D4
MFN -> D6 (PWM)
M21 -> D7
M22 -> D8
Button -> A6

Single Buttons: UP/DOWN/BACK/SELECT -> Forward/Backward/Left/Right
Combo (within 0.6s): UP+SELECT, UP+BACK, DOWN+SELECT, DOWN+BACK -> Diagonal moves
==========================================
*/

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

#define BUTTON_PIN A6

int speedValue = 180;
int slowSpeed = 90;

int lastButton = 0;
int firstPress = 0;
unsigned long firstPressTime = 0;
const unsigned long COMBO_WINDOW = 600;   // ms

String currentAction = "STOP";

//---------------------------------
int getButton()
{
  int value = analogRead(BUTTON_PIN);

  if (value >= 720 && value <= 745) return 3;   // BACK
  if (value >= 775 && value <= 805) return 2;   // DOWN
  if (value >= 840 && value <= 870) return 1;   // UP
  if (value >= 915 && value <= 945) return 4;   // SELECT

  return 0;
}

//---------------------------------
void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 0);
  currentAction = "STOP";
}

//---------------------------------
void singleMove(int btn)
{
  switch (btn)
  {
    case 1:   // UP - Forward
      digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
      digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
      analogWrite(MFN, speedValue);
      currentAction = "FORWARD";
      break;

    case 2:   // DOWN - Backward
      digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
      digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
      analogWrite(MFN, speedValue);
      currentAction = "BACKWARD";
      break;

    case 3:   // BACK - Left
      digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
      digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
      analogWrite(MFN, speedValue);
      currentAction = "LEFT";
      break;

    case 4:   // SELECT - Right
      digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
      digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
      analogWrite(MFN, speedValue);
      currentAction = "RIGHT";
      break;
  }

  Serial.println(currentAction);
}

//---------------------------------
void comboMove(int first, int second)
{
  bool forwardCombo = (first == 1);
  bool backwardCombo = (first == 2);
  bool rightSide = (second == 4);
  bool leftSide = (second == 3);

  if (forwardCombo && rightSide)
  {
    digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
    digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
    analogWrite(MFN, speedValue);
    currentAction = "COMBO: FORWARD-RIGHT";
  }
  else if (forwardCombo && leftSide)
  {
    digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
    digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
    analogWrite(MFN, slowSpeed);
    currentAction = "COMBO: FORWARD-LEFT";
  }
  else if (backwardCombo && rightSide)
  {
    digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
    digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
    analogWrite(MFN, speedValue);
    currentAction = "COMBO: BACKWARD-RIGHT";
  }
  else if (backwardCombo && leftSide)
  {
    digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
    digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
    analogWrite(MFN, slowSpeed);
    currentAction = "COMBO: BACKWARD-LEFT";
  }

  Serial.println(currentAction);
}

//---------------------------------
void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  stopCar();

  Serial.println("==================================");
  Serial.println(" MULTI-BUTTON ROBOT CONTROLLER ");
  Serial.println("==================================");
  Serial.println("Single: UP/DOWN/BACK/SELECT");
  Serial.println("Combo (within 0.6s): UP+SELECT, UP+BACK, DOWN+SELECT, DOWN+BACK");
}

//---------------------------------
void loop()
{
  int btn = getButton();

  if (btn != 0 && btn != lastButton)
  {
    if (firstPress != 0 && (millis() - firstPressTime) < COMBO_WINDOW && btn != firstPress)
    {
      comboMove(firstPress, btn);
      firstPress = 0;
    }
    else
    {
      singleMove(btn);
      firstPress = btn;
      firstPressTime = millis();
    }
  }

  if (btn == 0 && lastButton != 0)
  {
    stopCar();
    Serial.println("STOP");
  }

  if (firstPress != 0 && (millis() - firstPressTime) > COMBO_WINDOW)
  {
    firstPress = 0;
  }

  lastButton = btn;

  delay(50);
}