# include "mux.h"

int noR(int A, int B){// (A-R)/B
  int R = A%B;
return((A-R)/B);
};

MuxType M_injector ;
MuxType M_ground;
MuxType M_meas1;
MuxType M_meas2;

int payload[208];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  int inMin = 22; // Lowest input pin
  int inMax = 52; // Highest input pin
  for(int i=inMin; i<=inMax; i=i+2)
  {
    pinMode(i, OUTPUT);
  }

  M_meas1.SetPin(47,49,51,53);
  M_meas2.SetPin(37,39,41,43);

}

//22-52

uint8_t tranNode(uint8_t A){
  return 2*(A+11);
}

void getData_() {
  unsigned long currentMillis = millis();
  for (int injectN = 0; injectN < 16; injectN++) {
    uint8_t groundN = injectN + 1;

    //change value in mux
    uint8_t injectPin = tranNode(injectN);
    uint8_t groundPin = tranNode(groundN);

    //activate mux
    digitalWrite(injectPin,HIGH);
    digitalWrite(groundPin,LOW);
    

    for (int UnitMeas = 0; UnitMeas < 13; UnitMeas++) {
      int measN2 = (injectN + 3 + UnitMeas)% 16;
      int measN1 = (injectN + 2 + UnitMeas)% 16; //N1(+)-N2(-)
      Serial.println(String(injectN)+"  " +String(measN2)+"  "+String(measN1) );

      //change value in mux
      M_meas1.int_bina(measN1);
      M_meas2.int_bina(measN2);

      //activate mux
      M_meas1.activate_();
      M_meas2.activate_();

      //read from
      //payload[] = analogread(A14);
      Serial.println(analogRead(A14));
    }
    Serial.println("");
  }  
}

void getData() {
  for (int injectN = 0; injectN < 16; injectN++) {
    int groundN = injectN + 1;

    //change value in mux
    M_injector.int_bina(injectN);
    M_ground.int_bina(groundN);

    //activate mux
    M_injector.activate_();
    M_ground.activate_();

    for (int UnitMeas = 0; UnitMeas < 13; UnitMeas++) {
      int measN2 = (injectN + 3 + UnitMeas)% 16;
      int measN1 = (injectN + 2 + UnitMeas)% 16; //N1(+)-N2(-)
      Serial.println(String(injectN)+"  " +String(measN2)+"  "+String(measN1) );

      //change value in mux
      M_meas1.int_bina(measN1);
      M_meas2.int_bina(measN2);

      //activate mux
      M_meas1.activate_();
      M_meas2.activate_();

      //read from
      //payload[] = analogread(A14);
      Serial.println(analogRead(A14));
    }
    Serial.println("");
  }  
}

void loop(){
  
}
