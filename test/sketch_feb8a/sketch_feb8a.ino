void setup()
{
  Serial.begin(9600);
  pinMode(53,OUTPUT);
  pinMode(51,OUTPUT);
}

void loop()
{
  digitalWrite(53,HIGH);
  digitalWrite(51,HIGH);
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case '0' : digitalWrite(53,LOW);
        break;
      case '1' : digitalWrite(51,LOW);
        break;
      default: break;
    }
  }
delay(500);
}