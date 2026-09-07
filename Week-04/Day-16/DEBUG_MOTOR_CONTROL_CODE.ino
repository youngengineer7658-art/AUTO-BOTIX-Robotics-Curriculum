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

  Serial.println("===== MOTOR DEBUG MODE =====");
}

void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void forward()
{
  Serial.println("TEST : FORWARD");

  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  delay(3000);
}

void backward()
{
  Serial.println("TEST : BACKWARD");

  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  delay(3000);
}

void left()
{
  Serial.println("TEST : LEFT");

  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  delay(2000);
}

void right()
{
  Serial.println("TEST : RIGHT");

  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  delay(2000);
}

void loop()
{
  Serial.println("--- Starting Motor Test Cycle ---");

  
  forward();
  stopMotor();
  delay(1000);

  backward();
  stopMotor();
  delay(1000);

  left();
  stopMotor();
  delay(1000);

  right();
  stopMotor();
  delay(3000);

  Serial.println("===== ALL MOTOR TESTS PASSED =====");
}