/* 06/02/2020 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This sketch uses SDA/SCL on pins 20/21, respectively, and it uses the Ladybug STM32L432KC Breakout Board.
 The VL6180X is a time-of-flight ranging and ambient light sensor.
 
 Library may be used freely and without limit with attribution.
 
*/

#include "VL6180X.h"

#define I2C_BUS          Wire               // Define the I2C bus (Wire instance) you wish to use

I2Cdev                   i2c_0(&I2C_BUS);   // Instantiate the I2Cdev object and point to the desired I2C bus

// Define pins
#define myLed      13  // red led
#define intPin0     9
#define intPin1    10
#define intPin2    11
#define intPin3    12
#define resetPin0  A3
#define resetPin1  A2
#define resetPin2  A1
#define resetPin3  A0

const char        *build_date = __DATE__;   // 11 characters MMM DD YYYY
const char        *build_time = __TIME__;   // 8 characters HH:MM:SS

bool serialDebug = true;
  

volatile bool intFlag0 = false, intFlag1 = false, intFlag2 = false, intFlag3 = false;

// Configure VL6180X
// Choices are:
// VL6180Mode contRangeMode, contALSMode, or interLeaveMode (both range and als in succession)
// ALSGain alsGan40, alsGain20, alsGain10, alsGain5, alsGain2_5, alsGain1_67, alsGain1_25, alsGain1_0
uint8_t VL6180XMode = contRangeMode, ALSGain = alsGain20;
float als, realalsGain;
bool verboseMode = true;

VL6180X VL6180X(&i2c_0);


void setup()
{
  Serial.begin(115200);  
  delay(2000);
  Serial.println("Serial enabled!");

  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH);  // Start with led off, active LOW 

  pinMode(intPin0, INPUT); // active HIGH
  pinMode(intPin1, INPUT); // active HIGH
  pinMode(intPin2, INPUT); // active HIGH
  pinMode(intPin3, INPUT); // active HIGH

  I2C_BUS.begin();                        // Set master mode, default on SDA/SCL for STM32L4
  delay(1000);
  I2C_BUS.setClock(400000);               // I2C frequency at 400 kHz
  delay(1000);

  pinMode(resetPin0, OUTPUT);
  digitalWrite(resetPin0, LOW); // disable VL6180X_0 
  pinMode(resetPin2, OUTPUT);
  digitalWrite(resetPin2, LOW); // disable VL6180X_2
  pinMode(resetPin3, OUTPUT);
  digitalWrite(resetPin3, LOW); // disable VL6180X_3
  delay(100);
  VL6180X.setI2CAddress(VL6180X_DEFAULT_ADDRESS, 0x31); // reset VL6180X_1 I2C address to 0x31

  pinMode(resetPin2, INPUT);
  delay(100);
  VL6180X.setI2CAddress(VL6180X_DEFAULT_ADDRESS, 0x33); // reset VL6180X_2 I2C address to 0x33

  pinMode(resetPin3, INPUT);
  delay(100);
  VL6180X.setI2CAddress(VL6180X_DEFAULT_ADDRESS, 0x35); // reset VL6180X_3 I2C address to 0x35

  pinMode(resetPin0, INPUT);
  pinMode(resetPin1, INPUT);
  pinMode(resetPin2, INPUT);
  pinMode(resetPin3, INPUT);
  delay(100);
  
  Serial.println("Scan for I2C devices:");
  i2c_0.I2Cscan();                                      // should detect VL6180X at 0x31   
  delay(1000);
  
  // Read the WHO_AM_I register, this is a good test of communication
  Serial.println("VL6180X_0 proximity sensor...");
  uint8_t I2CAdd0 = VL6180X.getI2CAddress(VL6180X_0_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_0:  "); Serial.print("I am at "); Serial.print(I2CAdd0, HEX); Serial.print(" I should be at "); Serial.println(0x29, HEX);

  uint8_t DevID0 = VL6180X.getDevID(VL6180X_0_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_0:  "); Serial.print("I am device "); Serial.print(DevID0, HEX); Serial.print(" I should be "); Serial.println(0xB4, HEX);

  Serial.println("VL6180X_1 proximity sensor...");
  uint8_t I2CAdd1 = VL6180X.getI2CAddress(VL6180X_1_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_1:  "); Serial.print("I am at "); Serial.print(I2CAdd1, HEX); Serial.print(" I should be at "); Serial.println(0x31, HEX);

  uint8_t DevID1 = VL6180X.getDevID(VL6180X_1_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_1:  "); Serial.print("I am device "); Serial.print(DevID1, HEX); Serial.print(" I should be "); Serial.println(0xB4, HEX);

  Serial.println("VL6180X_2 proximity sensor...");
  uint8_t I2CAdd2 = VL6180X.getI2CAddress(VL6180X_2_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_2:  "); Serial.print("I am at "); Serial.print(I2CAdd2, HEX); Serial.print(" I should be at "); Serial.println(0x33, HEX);

  uint8_t DevID2 = VL6180X.getDevID(VL6180X_2_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_2:  "); Serial.print("I am device "); Serial.print(DevID2, HEX); Serial.print(" I should be "); Serial.println(0xB4, HEX);

  Serial.println("VL6180X_3 proximity sensor...");
  uint8_t I2CAdd3 = VL6180X.getI2CAddress(VL6180X_3_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_3:  "); Serial.print("I am at "); Serial.print(I2CAdd3, HEX); Serial.print(" I should be at "); Serial.println(0x35, HEX);

  uint8_t DevID3 = VL6180X.getDevID(VL6180X_3_ADDRESS);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X_3:  "); Serial.print("I am device "); Serial.print(DevID3, HEX); Serial.print(" I should be "); Serial.println(0xB4, HEX);

  Serial.println(" "); Serial.print("VL6180X_0 Info:"); Serial.println(" ");
  VL6180X.getDeviceInfo(VL6180X_0_ADDRESS);
  delay(100);
  Serial.println(" "); Serial.print("VL6180X_1 Info:"); Serial.println(" ");
  VL6180X.getDeviceInfo(VL6180X_1_ADDRESS);
  delay(100);
  Serial.println(" "); Serial.print("VL6180X_2 Info:"); Serial.println(" ");
  VL6180X.getDeviceInfo(VL6180X_2_ADDRESS);
  delay(100);
  Serial.println(" "); Serial.print("VL6180X_3 Info:"); Serial.println(" ");
  VL6180X.getDeviceInfo(VL6180X_3_ADDRESS);
  delay(100);
  
  if (DevID0 == 0xB4 && DevID1 == 0xB4 && DevID2 == 0xB4 && DevID3 == 0xB4) // VL6180X WHO_AM_I should always be 0xB4, four on the bus
  {  
    Serial.println(" "); 
    Serial.println("VL6180X_0 is online...");
    VL6180X.init(VL6180X_0_ADDRESS, VL6180XMode, ALSGain);
    Serial.println("VL6180X_0 is initialized...");
    
    realalsGain = VL6180X.setalsGain(ALSGain);
    uint8_t testALSGain0 =     VL6180X.getalsGain(VL6180X_0_ADDRESS);
    Serial.print("ALS Gain register is 0x"); Serial.println(testALSGain0, HEX);
    Serial.print("VL6180X_0 ALS Gain is "); Serial.println(realalsGain, 2);

    Serial.println("VL6180X_1 is online...");
    VL6180X.init(VL6180X_1_ADDRESS, VL6180XMode, ALSGain);
    Serial.println("VL6180X_1 is initialized...");
    
    uint8_t testALSGain1 =     VL6180X.getalsGain(VL6180X_2_ADDRESS);
    Serial.print("ALS Gain register is 0x"); Serial.println(testALSGain1, HEX);
    Serial.print("VL6180X_1 ALS Gain is "); Serial.println(realalsGain, 2); Serial.println(" ");

    Serial.println("VL6180X_2 is online...");
    VL6180X.init(VL6180X_2_ADDRESS, VL6180XMode, ALSGain);
    Serial.println("VL6180X_2 is initialized...");
    
    uint8_t testALSGain2 =     VL6180X.getalsGain(VL6180X_2_ADDRESS);
    Serial.print("ALS Gain register is 0x"); Serial.println(testALSGain2, HEX);
    Serial.print("VL6180X_2 ALS Gain is "); Serial.println(realalsGain, 2); Serial.println(" ");
    
    Serial.println("VL6180X_3 is online...");
    VL6180X.init(VL6180X_3_ADDRESS, VL6180XMode, ALSGain);
    Serial.println("VL6180X_3 is initialized...");
    
    uint8_t testALSGain3 =     VL6180X.getalsGain(VL6180X_3_ADDRESS);
    Serial.print("ALS Gain register is 0x"); Serial.println(testALSGain3, HEX);
    Serial.print("VL6180X_3 ALS Gain is "); Serial.println(realalsGain, 2); Serial.println(" ");
   }
  else
  {
    if(DevID0 != 0xB4) Serial.print("Could not connect to VL6180X_0: 0x"); Serial.println(DevID0, HEX);
    if(DevID1 != 0xB4) Serial.print("Could not connect to VL6180X_1: 0x"); Serial.println(DevID1, HEX);
    if(DevID2 != 0xB4) Serial.print("Could not connect to VL6180X_2: 0x"); Serial.println(DevID2, HEX);
    if(DevID3 != 0xB4) Serial.print("Could not connect to VL6180X_3: 0x"); Serial.println(DevID3, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }  

  attachInterrupt(intPin0, intHandler0, RISING);
  attachInterrupt(intPin1, intHandler1, RISING);
  attachInterrupt(intPin2, intHandler2, RISING);
  attachInterrupt(intPin3, intHandler3, RISING);
  VL6180X.clearInt(VL6180X_0_ADDRESS); //  clear interrupts
  VL6180X.clearInt(VL6180X_1_ADDRESS);
  VL6180X.clearInt(VL6180X_2_ADDRESS);  
  VL6180X.clearInt(VL6180X_3_ADDRESS);
  }  /* End of Setup */


void loop()
{
    // VL6180X_0
    if(intFlag0)
    {
       intFlag0 = false;
      
    // first check device status registers for errors 
    uint8_t status = VL6180X.getIntStatus(VL6180X_0_ADDRESS);  // read new sample data ready status register
   
    if(verboseMode) 
    {
      Serial.print("Status for VL6180X_0 = "); Serial.println(status);
    
      // first check for damage or error
      if(status & 0x40) Serial.println("laser safety error!");
      if(status & 0x80) Serial.println("PLL1 or PLL2 error!");
      Serial.println("  ");
    }
    
    if( !(status & 0x40) && !(status & 0x80) ) 
    {
      if(verboseMode) Serial.println("No errors...");
    
    // Check error status
      uint8_t error_status = VL6180X.getErrorStatus(VL6180X_0_ADDRESS);
    
      if(verboseMode) {
      if(error_status & 0x00) Serial.println("No errors...");
      if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
      if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
      if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
      if(error_status & 0x40) Serial.println("PLL1 Lock error!");
      if(error_status & 0x50) Serial.println("PLL2 Lock error!");
      if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
      if(error_status & 0x70) Serial.println("Max Convergence error!");
      if(error_status & 0x80) Serial.println("No Target Ignore error!");
      if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
      if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
      if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
      if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
      if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
      Serial.println("  ");
    }
    
    uint8_t range = VL6180X.getRange(VL6180X_0_ADDRESS);
    Serial.print("Current VL6180X_0 range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    VL6180X.clearInt(VL6180X_0_ADDRESS); // clear all data ready status interrupt        
    }

}

    // VL6180X_1

    if(intFlag1)
    {
       intFlag1 = false;
      
    // first check device status registers for errors 
    uint8_t status = VL6180X.getIntStatus(VL6180X_1_ADDRESS);  // read new sample data ready status register
   
    if(verboseMode) 
    {
      Serial.print("Status for VL6180X_1 = "); Serial.println(status);
    
      // first check for damage or error
      if(status & 0x40) Serial.println("laser safety error!");
      if(status & 0x80) Serial.println("PLL1 or PLL2 error!");
      Serial.println("  ");
    }
    
    if( !(status & 0x40) && !(status & 0x80) ) 
    {
      if(verboseMode) Serial.println("No errors...");
    
    // Check error status
      uint8_t error_status = VL6180X.getErrorStatus(VL6180X_1_ADDRESS);
    
      if(verboseMode) {
      if(error_status & 0x00) Serial.println("No errors...");
      if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
      if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
      if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
      if(error_status & 0x40) Serial.println("PLL1 Lock error!");
      if(error_status & 0x50) Serial.println("PLL2 Lock error!");
      if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
      if(error_status & 0x70) Serial.println("Max Convergence error!");
      if(error_status & 0x80) Serial.println("No Target Ignore error!");
      if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
      if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
      if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
      if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
      if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
      Serial.println("  ");
    }
    
    uint8_t range = VL6180X.getRange(VL6180X_1_ADDRESS);
    Serial.print("Current VL6180X_1 range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    VL6180X.clearInt(VL6180X_1_ADDRESS); // clear all data ready status interrupt        
    }

}


    // VL6180X_2

    if(intFlag2)
    {
       intFlag2 = false;
      
    // first check device status registers for errors 
    uint8_t status = VL6180X.getIntStatus(VL6180X_2_ADDRESS);  // read new sample data ready status register
   
    if(verboseMode) 
    {
      Serial.print("Status for VL6180X_2 = "); Serial.println(status);
    
      // first check for damage or error
      if(status & 0x40) Serial.println("laser safety error!");
      if(status & 0x80) Serial.println("PLL1 or PLL2 error!");
      Serial.println("  ");
    }
    
    if( !(status & 0x40) && !(status & 0x80) ) 
    {
      if(verboseMode) Serial.println("No errors...");
    
    // Check error status
      uint8_t error_status = VL6180X.getErrorStatus(VL6180X_2_ADDRESS);
    
      if(verboseMode) {
      if(error_status & 0x00) Serial.println("No errors...");
      if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
      if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
      if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
      if(error_status & 0x40) Serial.println("PLL1 Lock error!");
      if(error_status & 0x50) Serial.println("PLL2 Lock error!");
      if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
      if(error_status & 0x70) Serial.println("Max Convergence error!");
      if(error_status & 0x80) Serial.println("No Target Ignore error!");
      if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
      if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
      if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
      if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
      if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
      Serial.println("  ");
    }
    
    uint8_t range = VL6180X.getRange(VL6180X_2_ADDRESS);
    Serial.print("Current VL6180X_2 range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    VL6180X.clearInt(VL6180X_2_ADDRESS); // clear all data ready status interrupt        
    }

}


    // VL6180X_3

    if(intFlag3)
    {
       intFlag3 = false;
      
    // first check device status registers for errors 
    uint8_t status = VL6180X.getIntStatus(VL6180X_3_ADDRESS);  // read new sample data ready status register
   
    if(verboseMode) 
    {
      Serial.print("Status for VL6180X_3 = "); Serial.println(status);
    
      // first check for damage or error
      if(status & 0x40) Serial.println("laser safety error!");
      if(status & 0x80) Serial.println("PLL1 or PLL2 error!");
      Serial.println("  ");
    }
    
    if( !(status & 0x40) && !(status & 0x80) ) 
    {
      if(verboseMode) Serial.println("No errors...");
    
    // Check error status
      uint8_t error_status = VL6180X.getErrorStatus(VL6180X_3_ADDRESS);
    
      if(verboseMode) {
      if(error_status & 0x00) Serial.println("No errors...");
      if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
      if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
      if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
      if(error_status & 0x40) Serial.println("PLL1 Lock error!");
      if(error_status & 0x50) Serial.println("PLL2 Lock error!");
      if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
      if(error_status & 0x70) Serial.println("Max Convergence error!");
      if(error_status & 0x80) Serial.println("No Target Ignore error!");
      if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
      if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
      if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
      if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
      if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
      Serial.println("  ");
    }
    
    uint8_t range = VL6180X.getRange(VL6180X_3_ADDRESS);
    Serial.print("Current VL6180X_3 range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    VL6180X.clearInt(VL6180X_3_ADDRESS); // clear all data ready status interrupt        
    }

}
    
    digitalWrite(myLed, HIGH); delay(10); digitalWrite(myLed, LOW);
    delay(500);
    
}

//===================================================================================================================
//====== Set of useful function  
//===================================================================================================================

void intHandler0 ()
{
  intFlag0 = true;
}


void intHandler1 ()
{
  intFlag1 = true;
}


void intHandler2 ()
{
  intFlag2 = true;
}


void intHandler3 ()
{
  intFlag3 = true;
}
