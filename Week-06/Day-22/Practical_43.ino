#define M11 2
#define M12 4
#define MFN 6
#define M21 7
#define M22 8

void setup()
{
  Serial.begin(9600);

  pinMode(M11,OUTPUT);
  pinMode(M12,OUTPUT);
  pinMode(M21,OUTPUT);
  pinMode(M22,OUTPUT);
  pinMode(MFN,OUTPUT);

  stopMotor();

  Serial.println("Bluetooth Motor Control");
}

void motorON()
{
  analogWrite(MFN,255);

  digitalWrite(M11,HIGH);
  digitalWrite(M12,LOW);

  digitalWrite(M21,HIGH);
  digitalWrite(M22,LOW);
}

void stopMotor()
{
  analogWrite(MFN,0);

  digitalWrite(M11,LOW);
  digitalWrite(M12,LOW);

  digitalWrite(M21,LOW);
  digitalWrite(M22,LOW);
}

void loop()
{
  if(Serial.available())
  {
    char cmd=Serial.read();

    if(cmd=='1')
    {
      motorON();
      Serial.println("Motor ON");
    }

    if(cmd=='0')
    {
      stopMotor();
      Serial.println("Motor OFF");
    }
  }
}
