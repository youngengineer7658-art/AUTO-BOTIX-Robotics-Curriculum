// AUTO-BOTIX Robot Car
// Practical-28 : Left / Right Turn Test with Serial Monitor

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  digitalWrite(MFN, HIGH);

  Serial.println("===== LEFT / RIGHT TURN TEST =====");
}

void leftTurn()
{
  Serial.println("LEFT TURN");

  // Left Motor Stop
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  // Right Motor Forward
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void rightTurn()
{
  Serial.println("RIGHT TURN");

  // Left Motor Forward
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  // Right Motor Stop
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void stopMotor()
{
  Serial.println("STOP");

  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void loop()
{
  leftTurn();
  delay(3000);

  stopMotor();
  delay(1000);

  rightTurn();
  delay(3000);

  stopMotor();
  delay(1000);
}