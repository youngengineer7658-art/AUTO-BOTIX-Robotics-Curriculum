/*
==========================================
 Project-22 : Bluetooth Fan
 HC-05 TX -> D0
 HC-05 RX -> D1

 Motor Mapping
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

int speedValue = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  pinMode(MFN, OUTPUT);

  // Forward Direction
  digitalWrite(M11, HIGH);
  digitalWrite(M12, LOW);

  digitalWrite(M21, HIGH);
  digitalWrite(M22, LOW);

  analogWrite(MFN, 0);

  Serial.println("===============================");
  Serial.println(" Bluetooth Fan Control ");
  Serial.println("===============================");
  Serial.println("0=OFF");
  Serial.println("1=LOW");
  Serial.println("2=MEDIUM");
  Serial.println("3=HIGH");
  Serial.println("4=MAX");
}

void loop()
{
  if (Serial.available())
  {
    char cmd = Serial.read();

    switch(cmd)
    {
      case '0':
        speedValue = 0;
        Serial.println("Fan OFF");
        break;

      case '1':
        speedValue = 64;
        Serial.println("LOW Speed");
        break;

      case '2':
        speedValue = 128;
        Serial.println("MEDIUM Speed");
        break;

      case '3':
        speedValue = 192;
        Serial.println("HIGH Speed");
        break;

      case '4':
        speedValue = 255;
        Serial.println("MAX Speed");
        break;

      default:
        Serial.println("Invalid Command");
        break;
    }

    analogWrite(MFN, speedValue);
  }
}