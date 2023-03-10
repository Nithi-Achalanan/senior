const int MQ_9_pin = A0;
const int dht_pin = 6;
#include "DHT.h";
DHT dht(dht_pin, DHT11);

#include "PMS.h" ;
#include "Wire.h" ;
#include "SoftwareSerial.h" ;
SoftwareSerial mySerial(0, 1); // RX, TX 
PMS pms(mySerial);
PMS::DATA data;

class CO{
  public:
  int adc;
  void read();
  float analysis();
};
void CO::read(){
  adc = analogRead(MQ_9_pin);
};

float CO::analysis(){
  float slope = -0.4306765581;
  float A = 15.67182817;
  float Rseries = 1000;
  float V_Rseries = ((float)adc*5)/1023;
  float Rs = ((5-V_Rseries)/V_Rseries)*Rseries;
  float R0 = 809.52;
  float Y = Rs/R0;
  float CO_gas = pow(10,(log10(Y/A)/slope));
  return CO_gas; //ppm
}

class Fpayload{
  public:
  CO mq9;
  float CO {0};
  float Hum {0};
  float Temp {0};
  float PM25 {0};
  void get_all();
};

void Fpayload::get_all(){
  mq9.read();
  CO =  mq9.analysis();
  Hum = dht.readHumidity();
  Temp = dht.readTemperature();

  pms.requestRead();
  if (pms.readUntil(data)){
    PM25 = data.PM_AE_UG_2_5;
  }
  else{
    PM25 = 9999;
    Serial.println("pm disconnect");
  }
  pms.sleep();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();
  pms.passiveMode(); 
}

Fpayload AAA;

void loop() {
 
  AAA.get_all();
  Serial.print(" PM2.5 :  ");
  Serial.println(AAA.PM25);
  Serial.print(" CO :  ");
  Serial.println(AAA.CO);
  Serial.print( " h : ");
  Serial.println(AAA.Hum);
  Serial.print(" temp : ");
  Serial.println(AAA.Temp);
  Serial.println("");
  delay(10000);
}

void storage(){
  CO mq9;
  mq9.read();

  Serial.print(" CO :  ");
  Serial.print(mq9.analysis(),4);
  Serial.print( " h : ");
  float humidity = dht.readHumidity();
  Serial.print(humidity);
  Serial.print(" temp : ");
  float temperature = dht.readTemperature();
  Serial.println(temperature);
  
  delay(1000);
  
  pms.requestRead();
  if (pms.readUntil(data)){
    Serial.print("PM 1.0  (ug/m3): ");   Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM 2.5  (ug/m3): ");   Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");   Serial.println(data.PM_AE_UG_10_0);
    Serial.println();


  }

}