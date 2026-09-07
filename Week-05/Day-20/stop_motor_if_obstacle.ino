// Practical : Stop Motor if Obstacle

#define TRIG 5
#define ECHO A3

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

const float STOP_DISTANCE = 15.0;   // isse kam ho to motor stop

// -------- Read Distance --------
float getDistance() {

  long total = 0;

  for (int i = 0; i < 5; i++) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    total += pulseIn(ECHO, HIGH, 30000);

    delay(5);
  }

  return (total / 5.0) / 58.0;
}

void moveForward() {
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);
}

void stopMotor() {
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  digitalWrite(MFN, HIGH);

  Serial.begin(9600);
  Serial.println("===== STOP MOTOR IF OBSTACLE STARTED =====");
}

void loop() {

  float distance = getDistance();

  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.print(" cm   ");

  if (distance <= STOP_DISTANCE) {

    Serial.println("OBSTACLE DETECTED - MOTOR STOPPED");
    stopMotor();
  }
  else {

    Serial.println("PATH CLEAR - MOVING FORWARD");
    moveForward();
  }

  delay(150);
}