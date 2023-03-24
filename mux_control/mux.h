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
  s3 = r; //MSB
  //Serial.println(d);
}