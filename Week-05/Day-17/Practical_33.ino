// Practical : Read Ultrasonic Values

#define TRIG 5
#define ECHO A3

long duration;
float distance;
float lastDistance = -1;
const float THRESHOLD = 1.0;   // 1 cm se zyada change ho tabhi print karo

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);

  Serial.println("===== ULTRASONIC SENSOR TEST STARTED =====");
}

void loop() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0) {
    Serial.println("Out of Range");
  }
  else {
    distance = duration / 58.0;

    if (abs(distance - lastDistance) > THRESHOLD) {
      Serial.print("Duration = ");
      Serial.print(duration);

      Serial.print(" us    Distance = ");
      Serial.print(distance);
      Serial.println(" cm");

      lastDistance = distance;
    }
  }

  delay(100);
}
