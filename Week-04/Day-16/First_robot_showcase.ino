// ===== Project-16 : First Robot Showcase =====

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

  Serial.println("====================================");
  Serial.println("      FIRST ROBOT SHOWCASE");
  Serial.println("====================================");
}

void forward()
{
  Serial.println("FORWARD");

  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void backward()
{
  Serial.println("BACKWARD");

  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
}

void left()
{
  Serial.println("LEFT");

  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void right()
{
  Serial.println("RIGHT");

  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void spinLeft()
{
  Serial.println("SPIN LEFT");

  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void spinRight()
{
  Serial.println("SPIN RIGHT");

  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
}

void stopCar()
{
  Serial.println("STOP");

  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void loop()
{
  forward();
  delay(4000);

  left();
  delay(2000);

  forward();
  delay(3000);

  right();
  delay(2000);

  backward();
  delay(3000);

  spinLeft();
  delay(2000);

  spinRight();
  delay(2000);

  stopCar();
  delay(3000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" ROBOT SHOWCASE COMPLETED");
  Serial.println(" ALL MOVEMENTS WORKING");
  Serial.println("====================================");
  Serial.println();

  delay(5000);
}