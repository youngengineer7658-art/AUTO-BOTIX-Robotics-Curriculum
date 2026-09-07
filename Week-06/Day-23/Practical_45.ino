/*
==========================================
Practical-45 : Bluetooth Robot Car
HC-05 TX -> D0
HC-05 RX -> D1

Verified Motor Mapping
M11 -> D2
M12 -> D4
MFN -> D6 (PWM)
M21 -> D7
M22 -> D8
==========================================
*/

#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

int speedValue = 255;

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  analogWrite(MFN, speedValue);

  stopCar();

  Serial.println("===== Bluetooth Robot Car =====");
  Serial.println("F=Forward");
  Serial.println("B=Backward");
  Serial.println("L=Left");
  Serial.println("R=Right");
  Serial.println("S=Stop");
}

void forward()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  Serial.println("FORWARD");
}

void backward()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, HIGH);

  digitalWrite(M21, LOW);
  digitalWrite(M22, HIGH);

  Serial.println("BACKWARD");
}

void left()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  Serial.println("LEFT");
}

void right()
{
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  Serial.println("RIGHT");
}

void stopCar()
{
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);

  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);

  Serial.println("STOP");
}

void loop()
{
  if (Serial.available())
  {
    char cmd = toupper(Serial.read());

    switch(cmd)
    {
      case 'F':
        forward();
        break;

      case 'B':
        backward();
        break;

      case 'L':
        left();
        break;

      case 'R':
        right();
        break;

      case 'S':
        stopCar();
        break;
    }
  }
}
