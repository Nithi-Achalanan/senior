class MuxType{
  public: 
  int s0{0};
  int s1{0};
  int s2{0};
  int s3{0};

  uint8_t pin0{0};
  uint8_t pin1{0};
  uint8_t pin2{0};
  uint8_t pin3{0};

  void int_bina(uint8_t);  
  void activate_();
  void SetPin(uint8_t,uint8_t,uint8_t,uint8_t);
};

void MuxType::SetPin(uint8_t pin0_,uint8_t pin1_,uint8_t pin2_,uint8_t pin3_){
  pin0 = pin0_;
  pin1 = pin1_;
  pin2 = pin2_;
  pin3 = pin3_;
  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
}

void MuxType::activate_(){
  digitalWrite(pin0, s0);
  digitalWrite(pin1, s1);
  digitalWrite(pin2, s2);
  digitalWrite(pin3, s3);
}

void MuxType::int_bina(uint8_t A){ 
  int a;
  int b;
  int c;
  int d;
  int r;
  r = A%2;
  a = (A-r)/2;
  s0 = r; //LSB
  //Serial.println(a);
  r = a%2;
  b = (a - r)/2;
  s1 = r;
  //Serial.println(b);
  r = b%2;
  c = (b - r)/2;
  s2 = r;
  //Serial.println(c);

  r = c%2;
  d = (c-r)/2;
  s3 = d; //MSB
  //Serial.println(d);
}

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

  //มาเติมทีหลังด้วยย
  M_injector.SetPin();
  M_ground.SetPin();
  M_meas1.SetPin();
  M_meas2.SetPin();

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
      //payload[] = analogread(PinRead);
    }
    Serial.println("");
  }  
}

void loop(){
  
}
