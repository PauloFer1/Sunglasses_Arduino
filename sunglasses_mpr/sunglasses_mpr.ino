#include "mpr121.h"
#include <Wire.h>

int ledPin = 7;
int irqpin = 12;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states
char inData[20];
char inChar;
byte index = 0;
int incomingByte = 0;
int counter =1000;
int startCount =0;

void setup(){
  pinMode(irqpin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  
  Serial.begin(9600);
  Wire.begin();

  mpr121_setup();
  startCount =0;
}

void loop(){
  readTouchInputs();
  //Serial.println(counter);
  while(Serial.available() > 0)
  {
     startCount=1;
     char newC = (char)Serial.read();
     inData[index] = newC;
     
    if(inChar == '/' && newC == '0')
     {
     // Serial.println("EOF");
       processCommand(inData, index);
       inChar = -1;
       index=-1;
       memset(inData, 0, sizeof(inData));
       startCount=0,
       counter = 1000;
     }
     index++;
     inChar=newC;
  }
  
  if(startCount == 1 && counter > 0)
    counter= counter-1;
    
  if(counter == 0)
  {
   // Serial.println("reset");
    inChar = -1;
    index=0;
    memset(inData, 0, sizeof(inData));
    startCount=0,
    counter = 1000;
  }
}

void processCommand(char * cmd, int index)
{
  String val ="";
  String n = "";
  if(cmd[0]=='l' && cmd[1]=='n'){
    turnOnLight();
  }
  else if(cmd[0]=='l' && cmd[1]=='f' ){
    turnOffLight();
  }
  
  //**** DEBUG
  for(int i=0; i<index-1; i++)
  {
 //   Serial.print(cmd[i]); 
  }
//  Serial.println();
}

void readTouchInputs(){
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
          //pin i was just touched
          sendSerial(i);
          delay(400);
         // Serial.println(i);
        
        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }  
      
        touchStates[i] = 1;      
      }else{
        if(touchStates[i] == 1){
          
          //pin i is no longer being touched
       }
        
        touchStates[i] = 0;
      }
    
    }
    
  }
}
void turnOnLight()
{
  digitalWrite(ledPin, true);
}
void turnOffLight()
{
    digitalWrite(ledPin, false);
}

void sendSerial(int pin)
{
    if(pin == 0)
      Serial.println("al/0");
    else if(pin==1)
      Serial.println("ar/0");
    else if(pin == 2)
      Serial.println("au/0");
    else if(pin == 3)
      Serial.println("ad/0");
    else if(pin == 4)
      Serial.println("ak/0");
    else if(pin == 5)
      Serial.println("bk/0");
}


void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
