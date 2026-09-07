// ===== Practical : Test Performance =====

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

  Serial.println("===== ROBOT PERFORMANCE TEST =====");
}

void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void runTest(String testName, void (*action)(), int duration)
{
  Serial.print("Running: ");
  Serial.println(testName);

  unsigned long startTime = millis();

  action();          // motor ko HIGH/LOW set karo
  delay(duration);   // itni der chalao

  stopMotor();

  unsigned long endTime = millis();
  unsigned long actualTime = endTime - startTime;

  Serial.print("Expected Duration: ");
  Serial.print(duration);
  Serial.println(" ms");

  Serial.print("Actual Duration: ");
  Serial.print(actualTime);
  Serial.println(" ms");

  if (actualTime >= duration && actualTime <= (duration + 50)) {
    Serial.println("Result: PASS");
  } else {
    Serial.println("Result: FAIL (Timing Mismatch)");
  }

  Serial.println("-----------------------------");
}

void forwardAction()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void backwardAction()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);
  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
}

void leftAction()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void rightAction()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void loop()
{
  Serial.println("===== STARTING PERFORMANCE TEST CYCLE =====");

  runTest("FORWARD", forwardAction, 3000);
  delay(1000);

  runTest("BACKWARD", backwardAction, 3000);
  delay(1000);

  runTest("LEFT", leftAction, 2000);
  delay(1000);

  runTest("RIGHT", rightAction, 2000);
  delay(1000);

  Serial.println("===== ALL PERFORMANCE TESTS COMPLETED =====");
  Serial.println();

  delay(5000);   // agla cycle shuru hone se pehle pause
}
