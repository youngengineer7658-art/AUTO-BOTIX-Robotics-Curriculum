/*
   Practical-41 : HC-05 Send / Receive Test
   Hardware Serial
   HC-05 TX -> D0 (RX)
   HC-05 RX -> D1 (TX)
*/

void setup()
{
  Serial.begin(9600);

  delay(1000);

  Serial.println("=================================");
  Serial.println(" HC-05 SEND / RECEIVE TEST");
  Serial.println("=================================");
  Serial.println("Ready...");
}

void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();

    // PC ya Mobile se jo bhi aaya
    Serial.print("Received : ");
    Serial.println(c);

    // Wapas same character bhejo
    Serial.print("Echo : ");
    Serial.println(c);
  }
}