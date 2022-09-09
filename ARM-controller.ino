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

uint16_t e0_pos = 0;
AMT22 ENC0(ENC_0); //use default SPI pins
uint16_t e1_pos = 0;
AMT22 ENC0(ENC_1); //use default SPI pins

TMC2209Stepper driver_0(SW_RX, SW_TX, R_SENSE_0, DRIVER_ADDRESS_0);
TMC2209Stepper driver_1(SW_RX, SW_TX, R_SENSE_1, DRIVER_ADDRESS_1);
TMC2209Stepper driver_2(SW_RX, SW_TX, R_SENSE_2, DRIVER_ADDRESS_2);

bool hold_status = false; //false == unhold | true == hold

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
  // UART: Init SW UART (if selected) with default 115200 baudrate
  
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
}
  uint16_t E0Pos;
  uint16_t E1Pos;
  uint8_t attempts;
  int space = 0;
  for (;;) {
    if(space%10 == 0 ||space==0){
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

void loop() {
  while(1){
  encoderPosition = getPositionSPI(ENC_0, RES14);
  Serial.print("Encoder 0: ");
  Serial.print(encoderPosition, DEC); //print the position in decimal format
  Serial.write(NEWLINE);
/*encoderPosition = getPositionSPI(ENC_1, RES14);
  Serial.print("Encoder 1: ");
  Serial.print(encoderPosition, DEC); //print the position in decimal format
  Serial.write(NEWLINE);*/
  }

  
int stp = 0;
for (;;) {
  if(stp%10 == 0){
  encoderPosition = getPositionSPI(ENC_1, RES14);
 // y = map(encoderPosition, , 50, 50, -100);
  Serial.print("Encoder 1: ");
  Serial.println(encoderPosition, DEC);}
  if(encoderPosition >= 15300){
    shaft = !shaft;
    driver_2.shaft(true);
  }
  if(encoderPosition <= 5180){
    shaft = !shaft;
    driver_2.shaft(false);
  }
  ++t;
   /* digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(90);
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(90);*/
  }
  //Serial.println("DRIVER 2");
/*for (uint16_t i = 4000; i>0; i--) {
    digitalWrite(STEP_PIN_2, HIGH);
    delayMicroseconds(260);
    digitalWrite(STEP_PIN_2, LOW);
    delayMicroseconds(260);
  }*/

  
  /*shaft = !shaft;
  driver_0.shaft(shaft);
  driver_1.shaft(shaft);
  driver_2.shaft(shaft);
  
  delay(2000);*/
  
}



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
