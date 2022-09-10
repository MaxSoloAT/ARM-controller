//UART definitions
#define SW_RX            3
#define SW_TX            2

//Size of step
#define stp 10

//Stepper motor 0:
#define EN_PIN_0         A0  
#define DIR_PIN_0        A3  
#define STEP_PIN_0       4  
#define DRIVER_ADDRESS_0 0b00
#define R_SENSE_0 0.11f
#define M_CURRENT_0 1200
#define M_STEPPING_0 8

//Stepper motor 1:
#define EN_PIN_1         A1  
#define DIR_PIN_1        5  
#define STEP_PIN_1       6  
#define DRIVER_ADDRESS_1 0b01
#define R_SENSE_1 0.11f
#define M_CURRENT_1 1200
#define M_STEPPING_1 8

//Stepper motor 2:
#define EN_PIN_2         A2  
#define DIR_PIN_2        7  
#define STEP_PIN_2       8  
#define DRIVER_ADDRESS_2 0b10
#define R_SENSE_2 0.11f
#define M_CURRENT_2 1200
#define M_STEPPING_2 8

#define BAUDRATE        115200

#define ENC_0            9
#define ENC_1           10
#define SPI_MOSI        11
#define SPI_MISO        12
#define SPI_SCLK        13

#include "AMT22.h" 
#include <SPI.h>
#include <TMCStepper.h>
#include <EEPROM.h>

//!!!!!EEPROM READ!!!
uint16_t e0_ZERO = 0;
uint16_t e1_ZERO = 0;
//-------------------

uint16_t e0_pos = 0; //current position value 
AMT22 ENC0(ENC_0, SPI_MOSI, SPI_MISO, SPI_SCLK);   //use default SPI pins
uint16_t e1_pos = 0;
AMT22 ENC1(ENC_1, SPI_MOSI, SPI_MISO, SPI_SCLK);   //use default SPI pins

TMC2209Stepper driver_0(SW_RX, SW_TX, R_SENSE_0, DRIVER_ADDRESS_0);
TMC2209Stepper driver_1(SW_RX, SW_TX, R_SENSE_1, DRIVER_ADDRESS_1);
TMC2209Stepper driver_2(SW_RX, SW_TX, R_SENSE_2, DRIVER_ADDRESS_2);

int angle0;
int angle1;
int angle2; //not a angle mean open/close status or 0/255)

uint16_t sEnc0, eEnc0;
uint16_t sEnc1, eEnc1;

bool hold_status = false; //false == unhold | true == hold
bool new_cmd = false;
String rawData = "";
void setup() {
  Serial.begin(BAUDRATE);
  
  //driver 0 pins setting
  pinMode(EN_PIN_0, OUTPUT);
  pinMode(STEP_PIN_0, OUTPUT);
  pinMode(DIR_PIN_0, OUTPUT);
  digitalWrite(EN_PIN_0, LOW);
  
  //driver 1 pins setting
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);
  
  //driver 2 pins setting
  pinMode(EN_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_2, LOW);
  
  //SPI speed setting 
  SPI.setClockDivider(SPI_CLOCK_DIV32);  
  SPI.begin();                     

  driver_0.beginSerial(BAUDRATE);
  driver_1.beginSerial(BAUDRATE);
  driver_2.beginSerial(BAUDRATE);
  
  driver_0.begin();                 
  driver_1.begin();
  driver_2.begin();                 
 
  driver_0.toff(5);                         // Enables driver in software
  driver_0.rms_current(M_CURRENT_0);        // Set motor RMS current
  driver_0.microsteps(M_STEPPING_0);        // Set microsteps to 1/16th
  
  driver_1.toff(5);                         // Enables driver in software
  driver_1.rms_current(M_CURRENT_1);        // Set motor RMS current
  driver_1.microsteps(M_STEPPING_1);        // Set microsteps to 1/16th
  
  driver_2.toff(5);                         // Enables driver in software  
  driver_2.rms_current(M_CURRENT_2);        // Set motor RMS current
  driver_2.microsteps(M_STEPPING_2);        // Set microsteps to 1/16th

  //driver.en_pwm_mode(true);               // Toggle stealthChop on TMC2130/2160/5130/5160
  //driver.en_spreadCycle(false);           // Toggle spreadCycle on TMC2208/2209/2224
  driver_0.pwm_autoscale(true);             // Needed for stealthChop
  driver_1.pwm_autoscale(true);
  driver_2.pwm_autoscale(true);

  e0_pos = ENC0.getPosition();
  e1_pos = ENC1.getPosition();
  
}
  
void loop() {
  if(Serial.available()){
    new_cmd = true;
    rawData = Serial.readString();  //read until timeout
    rawData.trim();
    Serial.println(rawData);
  }
  if(new_cmd){
    new_cmd = false;
    ParseData(rawData);
    if(rawData == ""){
    
    }
  
  }
  else{
    //Serial.println(rawData);
  }
  
    
}



/*

  int space = 0;
  for (;;) {
    if(space%10 == 0 || space==0){
      E0Pos = getPositionSPI(ENC_0, RES14);
      Serial.println(E0Pos, DEC);
      if(E0Pos  < zeroE0){ 
        driver_1.shaft(true);
        if(abs(E0Pos - zeroE0)<=200){driver_1.microsteps(128);}
      }
      if(E0Pos > zeroE0){
        driver_1.shaft(false);
        if(abs(E0Pos - zeroE0)<=200){driver_1.microsteps(128);}
      } 
      E1Pos = getPositionSPI(ENC_1, RES14);
      Serial.println(E1Pos, DEC);
      if(E1Pos  < zeroE1){ 
        driver_2.shaft(false);
      }
      if(E1Pos > zeroE1){
        driver_2.shaft(true);
      } 
    }
     
    if(E0Pos != zeroE0){//E0Pos == zeroE0+4 || E0Pos == zeroE0-4|| 
    digitalWrite(STEP_PIN_0, HIGH);
    delayMicroseconds(90);
    digitalWrite(STEP_PIN_0, LOW);
    delayMicroseconds(90);
    }
    else{
    Serial.println("DONE0");
    driver_1.microsteps(8);
    
    }

    if(E1Pos != zeroE1){//E0Pos == zeroE0+4 || E0Pos == zeroE0-4|| 
      
    digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(90);
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(90);
    }
    else{
    Serial.println("DONE1");}
    space++;
  
}
}

*/

//go driver 2 to move motor and open the arm into the hole in ceil.
void Hold(){
  digitalWrite(STEP_PIN_2, HIGH);
  delayMicroseconds(260);
  digitalWrite(STEP_PIN_2, LOW);
  delayMicroseconds(260);

  hold_status = true;
}

//!Hold
void unHold(){

  hold_status = false;
}

void ParseData(const String &Data){
  String cmd = "";
  
  Serial.println(Data);
  if (Data == "SETZERO"){
    // set all motor to zero position
    Serial.println("ZERO");
    setZero();
    return;
  }  
  if (Data == "STATUS"){
    //get current angle a0 a1 and holder status
    // line A0:250 A1:100 A2:255 
    Serial.println("STATS....");
    Serial.print("e0:");
     Serial.println(ENC0.getPosition());
     Serial.print("e1:");
     Serial.println(ENC1.getPosition());
     return;
  }
  
  int sep = Data.indexOf(":");
  cmd = Data.substring(0,sep);
 // Serial.println(cmd);
  if (cmd == "A0"){
    int sep1 = Data.indexOf(" ");
    String A0 = Data.substring(sep+1,sep1);
    //toInt();
    //Serial.print("a0: ");
    Serial.println(A0);
    angle0 = A0.toInt();
    
    int sep2 = Data.indexOf(":", sep1);
    //Serial.println(sep2);
    int sep3 = Data.indexOf(" ", sep2);
    //Serial.println(sep3);
    String A1 = Data.substring(sep2+1,sep3);
    Serial.println(A1);
    angle1 = A1.toInt();
    
    int sep4 = Data.indexOf(":", sep3);
    String A2 = Data.substring(sep4+1, Data[Data.length()-1]);
    Serial.println(A2);
    angle2 = A2.toInt();
    setAngles(angle0, angle1, angle2);
  }
    
}
//go motors to set angle depending to encoders
void setAngles(int angle0, int angle1, int angle2){
  
  int E0_target = conANGtoENC(sEnc0, eEnc0, angle0);
  Serial.print("E0 target: ");
  Serial.println(E0_target);
  int E1_target = conANGtoENC(sEnc1, eEnc1,angle1);
  Serial.print("E1 target: ");
  Serial.println(E1_target);  
  
  
  if(angle2 == 255){
    Hold();
  }
  else{
    unHold();
  }
}

// convert real angle to encoder data so 0-359 to 0-16k
uint16_t conANGtoENC(uint16_t startA, uint16_t endA, uint16_t A){ //program 2 value or 4 for 2 encoders. use like stopers(avalible enc(angle) from 1000 to 15000.)
  //1 dagree = 360/uint16_t
  // convert angle A to enc data
  
  float E = float(A)*16384.0/360.0;
  if((startA >E)||(endA<E)){Serial.println("!!!!!!OUT OF RANGE!!!!!!");}
  return E;  
}


void setZero(){
  e0_ZERO = ENC0.getPosition();
  e1_ZERO = ENC1.getPosition();
  Serial.print("e0_ZERO:");
  Serial.println(e0_ZERO);
  Serial.print("e1_ZERO:");
  Serial.println(e1_ZERO);

}
