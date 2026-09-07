/*
==========================================
Practical-66 : Create Combined Movement Commands
Autobotix Robotics Board

Sequence Combos (dabao ek ke baad ek, 600ms ke andar):
UP + SELECT  -> Forward-Right (diagonal)
UP + BACK    -> Forward-Left (diagonal)
DOWN + SELECT-> Backward-Right (diagonal)
DOWN + BACK  -> Backward-Left (diagonal)
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
      Serial.println("FORWARD");
      break;

    case 2:   // DOWN - Backward
      digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
      digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
      analogWrite(MFN, speedValue);
      Serial.println("BACKWARD");
      break;

    case 3:   // BACK - Left
      digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
      digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
      analogWrite(MFN, speedValue);
      Serial.println("LEFT");
      break;

    case 4:   // SELECT - Right
      digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
      digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
      analogWrite(MFN, speedValue);
      Serial.println("RIGHT");
      break;
  }
}

//---------------------------------
// Diagonal combo: dono motors forward/backward, ek motor slow (turn create karta hai)
//---------------------------------
void comboMove(int first, int second)
{
  bool forwardCombo = (first == 1);    // UP se shuru hua tha
  bool backwardCombo = (first == 2);   // DOWN se shuru hua tha
  bool rightSide = (second == 4);      // SELECT = right side slow (right turn)
  bool leftSide = (second == 3);       // BACK = left side slow (left turn)

  if (forwardCombo && rightSide)
  {
    digitalWrite(M11, HIGH); digitalWrite(M12, LOW);   // left motor full
    digitalWrite(M21, HIGH); digitalWrite(M22, LOW);   // right motor slow
    analogWrite(MFN, speedValue);
    Serial.println("COMBO: FORWARD-RIGHT (diagonal)");
  }
  else if (forwardCombo && leftSide)
  {
    digitalWrite(M11, HIGH); digitalWrite(M12, LOW);
    digitalWrite(M21, HIGH); digitalWrite(M22, LOW);
    analogWrite(MFN, slowSpeed);
    Serial.println("COMBO: FORWARD-LEFT (diagonal)");
  }
  else if (backwardCombo && rightSide)
  {
    digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
    digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
    analogWrite(MFN, speedValue);
    Serial.println("COMBO: BACKWARD-RIGHT (diagonal)");
  }
  else if (backwardCombo && leftSide)
  {
    digitalWrite(M11, LOW); digitalWrite(M12, HIGH);
    digitalWrite(M21, LOW); digitalWrite(M22, HIGH);
    analogWrite(MFN, slowSpeed);
    Serial.println("COMBO: BACKWARD-LEFT (diagonal)");
  }
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

  Serial.println("===== COMBINED MOVEMENT COMMANDS =====");
  Serial.println("Single: UP/DOWN/BACK/SELECT");
  Serial.println("Combo (within 0.6s): UP+SELECT, UP+BACK, DOWN+SELECT, DOWN+BACK");
}

//---------------------------------
void loop()
{
  int btn = getButton();

  // Naya button press detect hua
  if (btn != 0 && btn != lastButton)
  {
    if (firstPress != 0 && (millis() - firstPressTime) < COMBO_WINDOW && btn != firstPress)
    {
      // Combo detected!
      comboMove(firstPress, btn);
      firstPress = 0;   // reset combo tracking
    }
    else
    {
      // Pehla press - single movement karo, aur combo window shuru karo
      singleMove(btn);
      firstPress = btn;
      firstPressTime = millis();
    }
  }

  // Sab buttons release ho gaye
  if (btn == 0 && lastButton != 0)
  {
    stopCar();
    Serial.println("STOP");
  }

  // Combo window expire ho gayi bina second press ke
  if (firstPress != 0 && (millis() - firstPressTime) > COMBO_WINDOW)
  {
    firstPress = 0;
  }

  lastButton = btn;

  delay(50);
}