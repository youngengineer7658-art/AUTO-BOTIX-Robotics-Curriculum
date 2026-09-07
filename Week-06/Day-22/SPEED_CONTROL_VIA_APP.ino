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

  digitalWrite(M11,HIGH);
  digitalWrite(M12,LOW);

  digitalWrite(M21,HIGH);
  digitalWrite(M22,LOW);

  analogWrite(MFN,0);

  Serial.println("Bluetooth Speed Control");
}

void loop()
{
  if(Serial.available())
  {
    char cmd=Serial.read();

    switch(cmd)
    {
      case '0':
        analogWrite(MFN,0);
        Serial.println("STOP");
      break;

      case '1':
        analogWrite(MFN,64);
        Serial.println("25%");
      break;

      case '2':
        analogWrite(MFN,128);
        Serial.println("50%");
      break;

      case '3':
        analogWrite(MFN,192);
        Serial.println("75%");
      break;

      case '4':
        analogWrite(MFN,255);
        Serial.println("100%");
      break;
    }
  }
}