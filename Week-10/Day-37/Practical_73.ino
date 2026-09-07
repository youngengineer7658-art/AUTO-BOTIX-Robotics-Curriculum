// ===============================================
// Practical-73 : Line Following Basics with Serial
// ===============================================

// Motor Driver Pins
#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

// IR Line Sensors
#define LEFT_SENSOR  A0
#define RIGHT_SENSOR A1

// Non-blocking Serial Timer
unsigned long lastSerialTime = 0;
const char* robotAction = "STOP";

void setup()
{
  Serial.begin(9600);
  delay(100);

  Serial.println(F("\n========================================"));
  Serial.println(F("  PRACTICAL-73: LINE FOLLOWER READY     "));
  Serial.println(F("========================================"));

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(MFN, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  stopMotor();
}

void loop()
{
  int left = digitalRead(LEFT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  // Logic: Black Line = 0, White Surface = 1

  if (left == 1 && right == 1)
  {
    // Both on White -> Line is in center -> Go Straight
    forward();
  }
  else if (left == 0 && right == 1)
  {
    // Left sensor hits Black Line -> Steer Left
    leftTurn();
  }
  else if (left == 1 && right == 0)
  {
    // Right sensor hits Black Line -> Steer Right
    rightTurn();
  }
  else
  {
    // Both sensors hit Black (End of track / T-junction)
    stopMotor();
  }

  // Serial Monitor Telemetry (Updates smoothly every 150ms)
  if (millis() - lastSerialTime >= 150)
  {
    lastSerialTime = millis();
    
    Serial.print(F("LEFT: "));
    Serial.print(left == 1 ? F("WHITE (1)") : F("BLACK (0)"));
    Serial.print(F("  |  RIGHT: "));
    Serial.print(right == 1 ? F("WHITE (1)") : F("BLACK (0)"));
    Serial.print(F("  |  ACTION: "));
    Serial.println(robotAction);
  }

  delay(10);
}

// =====================
// Motor Functions
// =====================

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 170); // Cruising Speed
  robotAction = "FORWARD";
}

void leftTurn()
{
  // Pivot Left Turn
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);
  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 150);
  robotAction = "TURNING LEFT";
}

void rightTurn()
{
  // Pivot Right Turn
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);
  analogWrite(MFN, 150);
  robotAction = "TURNING RIGHT";
}

void stopMotor()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
  analogWrite(MFN, 0);
  robotAction = "STOP";
}
