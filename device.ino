# include "mux.h"
MuxType M_injector ;
MuxType M_ground;
MuxType M_meas1;
MuxType M_meas2;

void getData(int*& PVpayload) {
  int count = 0;
  for (int injectN = 0; injectN < 16; injectN++) {
    int groundN = (injectN + 1)%16 ;

    //change value in mux
    M_injector.int_bina(injectN);
    M_ground.int_bina(groundN);

    //activate mux
    M_injector.activate_();
    M_ground.activate_();
    delay(30);
    for (int UnitMeas = 0; UnitMeas < 16  ; UnitMeas++) {
      if (UnitMeas != (injectN -1)%16  && UnitMeas != (injectN)%16 && UnitMeas != (injectN +1)%16 && (UnitMeas + 1)%16 != injectN ){
 
        int measN2 = (UnitMeas + 1)%16 ;
        int measN1 = UnitMeas;
        delay(2);
        //Serial.println(String(injectN)+"  " +String(measN2)+"  "+String(measN1) );

        //change value in mux
        M_meas1.int_bina(measN1);
        M_meas2.int_bina(measN2);

        //activate mux
        M_meas1.activate_();
        M_meas2.activate_();

        //read from
        //payload[] = analogread(A14);
        //int Write_Ser = Serial.println(analogRead(A14));
        PVpayload[count] = analogRead(A14) ;
        count++;
      }
    }
    //Serial.println("");
  }  
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  M_injector.SetPin(30,31,32,33);
  M_ground.SetPin(36,37,38,39);
  M_meas1.SetPin(42,43,44,45);
  M_meas2.SetPin(48,49,50,51);

}

void loop() {
  // check if there's any data available on the serial line:
  if (Serial.available() >= 4) { // check for at least 4 bytes (the size of the command)
    // read the incoming data into a buffer:
    byte buffer[4];
    Serial.readBytes(buffer, 4);

    // check which command was received:

    switch (buffer[0]) {
      case 1:  //1,2,50,54 check status
        if (buffer[1] == 2 && buffer[2] == 50 && buffer[3] == 54) {
          pinMode(LED_BUILTIN, OUTPUT);
          onlineState();
        }
        break;

      case 0x02: //2,1,50,44 getdata
        if (buffer[1] == 2 && buffer[2] == 50 && buffer[3] == 44) {
          int* payload {new int[208]};
          getData(payload);
          startGet();
          Serial.write(208);
          
          
          //delay(30);
          //sentTest();
          //sentHeader();
          
          for (int i = 0; i < 208; i++) {
            //Serial.write( i+300 >> 8);    
            //Serial.write( i+300 & 0xFF);
            Serial.write( payload[i] >> 8);    
            Serial.write( payload[i] & 0xFF);
          }
          
          sentTailer();
        }
        break;
      // add more cases for other commands here
      default:
        massageError();
        break;
    }
  }
}

// format of massage is 1 , 5 , 4 , 3 , len(other massage  ->), other , 1  , 1 (stop codon)

// 1 , 5 , 4 , 3 header
// len(other massage  ->), other 
// 1 , 1  (stop codon)

void massageError() {
  int error[8] = {1,5,4,3, 1,50, 1,1}; //50 = mean massage error
  for(int i = 0; i<8; i++){
    Serial.write(error[i]);    
  }
}

void onlineState() {
  int check[8] = {1,5,4,3, 1,49, 1,1}; //49 = mean massage error
  for(int i = 0; i<8; i++){
    Serial.write(check[i]);  
  }
}

void startGet() {
  int start[8] = {1,5,4,3}; //, 1,48, 1,1}; //48 = start reconstruction
  for(int i = 0; i<4; i++){
    Serial.write(start[i]);  
  }
}

void blink() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}

void sentTailer(){
  Serial.write(1);
  Serial.write(1);  
}

void sentHeader() {
  int header[4] = {1,5,4,3};
  for(int i = 0; i<4; i++){
    Serial.write(header[i]);    
  }
}
void sentTest(){
  int randomArray[208];
  for (int i = 0; i < 208; i++) {
    randomArray[i] = random(550); // generate a random number between 0 and 549
    Serial.write(randomArray[i] >> 8);    
    Serial.write(randomArray[i] & 0xFF);
  }
}