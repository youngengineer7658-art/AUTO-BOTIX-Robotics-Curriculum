/*
===================================================
Project-37 : Line Following Robot with Serial Output
===================================================
*/

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

#define LEFT_IR  A0
#define RIGHT_IR A1

unsigned long lastSerialTime = 0;
const char* robotStatus = "STOP";

void setup()
{
  Serial.begin(9600);
  delay(100);

  Serial.println(F("\n========================================"));
  Serial.println(F("   PROJECT-37: LINE FOLLOWER READY      "));
  Serial.println(F("========================================"));

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(MFN, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);

  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  stopRobot();
}

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 170); // Smooth cruising speed
  robotStatus = "FORWARD";
}

void left()
{
  // Left motor Reverse, Right motor Forward (Sharp Pivot Turn)
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 150);
  robotStatus = "TURNING LEFT";
}

void right()
{
  // Left motor Forward, Right motor Reverse (Sharp Pivot Turn)
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  analogWrite(MFN, 150);
  robotStatus = "TURNING RIGHT";
}

void stopRobot()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);
  robotStatus = "STOP";
}

void loop()
{
  int leftSensor = digitalRead(LEFT_IR);
  int rightSensor = digitalRead(RIGHT_IR);

  // WHITE = HIGH (1), BLACK = LOW (0)

  // 1. Line is in center (Both on White) -> Move Forward
  if (leftSensor == HIGH && rightSensor == HIGH)
  {
    forward();
  }
  // 2. Left sensor touches Black Line -> Turn Left
  else if (leftSensor == LOW && rightSensor == HIGH)
  {
    left();
  }
  // 3. Right sensor touches Black Line -> Turn Right
  else if (leftSensor == HIGH && rightSensor == LOW)
  {
    right();
  }
  // 4. Both on Black Line (Stop Mark / T-Junction) -> Stop
  else
  {
    stopRobot();
  }

  // Serial Monitor Output (Every 150ms smoothly)
  if (millis() - lastSerialTime >= 150)
  {
    lastSerialTime = millis();
    
    Serial.print(F("LEFT: "));
    Serial.print(leftSensor == HIGH ? F("WHITE (1)") : F("BLACK (0)"));
    Serial.print(F("  |  RIGHT: "));
    Serial.print(rightSensor == HIGH ? F("WHITE (1)") : F("BLACK (0)"));
    Serial.print(F("  |  ACTION: "));
    Serial.println(robotStatus);
  }

  delay(10);
}