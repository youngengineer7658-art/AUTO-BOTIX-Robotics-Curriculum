// ===================================
// Practical-74 : Test Line Sensors
// ===================================

// Line Sensor Pins
#define LEFT_SENSOR  A0
#define RIGHT_SENSOR A1

void setup()
{
  Serial.begin(9600);

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  Serial.println("=== Line Sensor Test ===");
}

void loop()
{
  int left = digitalRead(LEFT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  Serial.print("Left Sensor : ");
  Serial.print(left);

  if (left == LOW)
    Serial.print(" (BLACK)");
  else
    Serial.print(" (WHITE)");

  Serial.print("    ");

  Serial.print("Right Sensor : ");
  Serial.print(right);

  if (right == LOW)
    Serial.println(" (BLACK)");
  else
    Serial.println(" (WHITE)");

  delay(200);
}
