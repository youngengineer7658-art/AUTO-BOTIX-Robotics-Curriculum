/*
==========================================
Practical-65 : Control Robot Using Multiple Buttons
Autobotix Robotics Board (Dual Enable Fixed)

M11  -> D2
M12  -> D4
MFN  -> D6 (Motor 1 PWM)
M2EN -> D9 (Motor 2 PWM)
M21  -> D7
M22  -> D8
Button -> A6
==========================================
*/

#define M11  2
#define M12  4
#define MFN  6      // Motor 1 Enable / PWM
#define M2EN 9      // Motor 2 Enable / PWM
#define M21  7
#define M22  8

#define BUTTON_PIN A6

// Calibrated Hardware ADC Ranges
#define BTN_BACK_MIN   725
#define BTN_BACK_MAX   745
#define BTN_DOWN_MIN   780
#define BTN_DOWN_MAX   800
#define BTN_UP_MIN     845
#define BTN_UP_MAX     865
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945

int speedValue = 180;
int lastButton = 0;

//---------------------------------
int getButton()
{
  int value = analogRead(BUTTON_PIN);

  if (value >= BTN_BACK_MIN && value <= BTN_BACK_MAX)     return 3;   // BACK (733-734)
  if (value >= BTN_DOWN_MIN && value <= BTN_DOWN_MAX)     return 2;   // DOWN (789-790)
  if (value >= BTN_UP_MIN && value <= BTN_UP_MAX)         return 1;   // UP (853-855)
  if (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX) return 4;   // SELECT (931-932)

  return 0;
}

//---------------------------------
void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
  analogWrite(MFN, speedValue);
  analogWrite(M2EN, speedValue);
  Serial.println("FORWARD");
}

//---------------------------------
void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);
  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
  analogWrite(MFN, speedValue);
  analogWrite(M2EN, speedValue);
  Serial.println("BACKWARD");
}

//---------------------------------
void leftTurn()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
  analogWrite(MFN, speedValue);
  analogWrite(M2EN, speedValue);
  Serial.println("LEFT");
}

//---------------------------------
void rightTurn()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
  analogWrite(MFN, speedValue);
  analogWrite(M2EN, speedValue);
  Serial.println("RIGHT");
}

//---------------------------------
void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 0);
  analogWrite(M2EN, 0);
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
  pinMode(M2EN, OUTPUT);

  stopCar();

  Serial.println("===== MULTI BUTTON ROBOT CONTROL =====");
  Serial.println("UP = Forward, DOWN = Backward");
  Serial.println("BACK = Left, SELECT = Right");
}

//---------------------------------
void loop()
{
  int btn = getButton();

  if (btn != 0 && btn != lastButton)
  {
    switch (btn)
    {
      case 1: forward();   break;   // UP
      case 2: backward();  break;   // DOWN
      case 3: leftTurn();  break;   // BACK
      case 4: rightTurn(); break;   // SELECT
    }
  }

  if (btn == 0 && lastButton != 0)
  {
    stopCar();
    Serial.println("STOP");
  }

  lastButton = btn;

  delay(50);
}