// Practical : Motor Speed Levels using PWM

#define M1     2
#define M2     4
#define M_EN   6      // PWM pin
#define BTN    A6

// PWM duty cycle values for each speed level (0-255 range)
int speedLevels[] = {0, 85, 170, 255};        // OFF, LOW, MEDIUM, HIGH
String levelNames[] = {"OFF", "LOW", "MEDIUM", "HIGH"};
const int totalLevels = 4;

int currentLevel = 0;
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M_EN, OUTPUT);

  // Fix motor direction (Forward) — sirf speed change hogi, direction nahi
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);

  // Start at OFF
  analogWrite(M_EN, speedLevels[currentLevel]);

  Serial.println("===== MOTOR SPEED LEVELS STARTED =====");
  Serial.println("Level: OFF | PWM: 0");
}

void loop() {

  int button = analogRead(BTN);

  // Button released (idle value ~0)
  if (button < 200) {
    buttonPressed = false;
  }

  // SELECT (931 - 932) -> New press detected — move to next speed level
  else if (button >= 920 && button <= 945 && !buttonPressed) {

    currentLevel = (currentLevel + 1) % totalLevels;   // cycle back to 0 after HIGH

    analogWrite(M_EN, speedLevels[currentLevel]);

    Serial.print("Level: ");
    Serial.print(levelNames[currentLevel]);
    Serial.print(" | PWM: ");
    Serial.println(speedLevels[currentLevel]);

    buttonPressed = true;
  }

  delay(20);
}