// ===== Practical : Predefined Movement =====

#define M11  2
#define M12  4
#define MFN  6      // Motor 1 Enable
#define M2EN 9      // Motor 2 Enable
#define M21  7
#define M22  8
#define BTN  A6

// Calibrated Range for SELECT Button (931 - 932)
#define BTN_SELECT_MIN 920
#define BTN_SELECT_MAX 945

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);
  pinMode(M2EN, OUTPUT);
  pinMode(BTN, INPUT);

  digitalWrite(MFN, HIGH);
  digitalWrite(M2EN, HIGH);

  Serial.println("===== PREDEFINED MOVEMENT STARTED =====");
  Serial.println("Press SELECT Button to start movement sequence");
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

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void runSequence()
{
  // Predefined Path: Forward -> Right -> Forward -> Left -> Forward -> Stop

  forward();
  delay(2000);
  stopCar();
  delay(500);

  right();
  delay(1000);
  stopCar();
  delay(500);

  forward();
  delay(2000);
  stopCar();
  delay(500);

  left();
  delay(1000);
  stopCar();
  delay(500);

  forward();
  delay(2000);
  stopCar();
}

void loop()
{
  int value = analogRead(BTN);

  // SELECT Button (931 - 932) dabane par pura predefined path chalega
  if (value >= BTN_SELECT_MIN && value <= BTN_SELECT_MAX) {
    runSequence();

    // Button release hone ka wait karo (idle < 200)
    while (analogRead(BTN) > 200);
    delay(150);
  }
}
