/* 06/02/2020 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This sketch uses SDA/SCL on pins 20/21, respectively, and it uses the Ladybug STM32L432KC Breakout Board.
 The VL6180X is a time-of-flight ranging and ambient light sensor.
 
 Library may be used freely and without limit with attribution.
 
*/
  
#ifndef VL6180X_h
#define VL6180X_h

#include "Arduino.h"
#include <Wire.h>
#include "I2Cdev.h"

// VL6180X Data Sheet and Register Map Register Map
// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00112632.pdf
// The VL6180X uses 16-bit register addresses rather than the usual 8-bit (2-byte) address found in most
// other sensors. This requires a slight modification to the read and write functions but is straightforward.
//
// VL6180X registers
#define VL6180X_WHO_AM_I                             0x0000   // should be 0xB4              
#define VL6180X_IDENTIFICATION_MODEL_ID              0x0000
#define VL6180X_IDENTIFICATION_MODEL_REV_MAJOR       0x0001
#define VL6180X_IDENTIFICATION_MODEL_REV_MINOR       0x0002
#define VL6180X_IDENTIFICATION_MODULE_REV_MAJOR      0x0003
#define VL6180X_IDENTIFICATION_MODULE_REV_MINOR      0x0004
#define VL6180X_IDENTIFICATION_DATE_HI               0x0006
#define VL6180X_IDENTIFICATION_DATE_LO               0x0007
#define VL6180X_IDENTIFICATION_TIME_HI               0x0008
#define VL6180X_IDENTIFICATION_TIME_LO               0x0009
#define VL6180X_SYSTEM_MODE_GPIO0                    0x0010
#define VL6180X_SYSTEM_MODE_GPIO1                    0x0011
#define VL6180X_SYSTEM_HISTORY_CTRL                  0x0012
#define VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014
#define VL6180X_SYSTEM_INTERRUPT_CLEAR               0x0015
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET            0x0016
#define VL6180X_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017
#define VL6180X_SYSRANGE_START                       0x0018
#define VL6180X_SYSRANGE_THRESH_HIGH                 0x0019
#define VL6180X_SYSRANGE_THRESH_LOW                  0x001A
#define VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001B
#define VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME        0x001C
#define VL6180X_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001E
#define VL6180X_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021
#define VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022
#define VL6180X_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024
#define VL6180X_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025
#define VL6180X_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026
#define VL6180X_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002C
#define VL6180X_SYSRANGE_RANGE_CHECK_ENABLES         0x002D
#define VL6180X_SYSRANGE_VHV_RECALIBRATE             0x002E
#define VL6180X_SYSRANGE_VHV_REPEAT_RATE             0x0031
#define VL6180X_SYSALS_START                         0x0038
#define VL6180X_SYSALS_THRESH_HIGH                   0x003A
#define VL6180X_SYSALS_THRESH_LOW                    0x003C
#define VL6180X_SYSALS_INTERMEASUREMENT_PERIOD       0x003E
#define VL6180X_SYSALS_ANALOGUE_GAIN                 0x003F
#define VL6180X_SYSALS_INTEGRATION_PERIOD            0x0040
#define VL6180X_RESULT_RANGE_STATUS                  0x004D
#define VL6180X_RESULT_ALS_STATUS                    0x004E
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO         0x004F
#define VL6180X_RESULT_ALS_VAL                       0x0050
#define VL6180X_RESULT_HISTORY_BUFFER0               0x0052 // This is a FIFO buffer that can store 8 range values or 16 ALS values
#define VL6180X_RESULT_HISTORY_BUFFER1               0x0053 // It would be read in burst mode so all that is
#define VL6180X_RESULT_HISTORY_BUFFER2               0x0054 // needed would be to reference the first address
#define VL6180X_RESULT_HISTORY_BUFFER3               0x0055
#define VL6180X_RESULT_HISTORY_BUFFER4               0x0056
#define VL6180X_RESULT_HISTORY_BUFFER5               0x0057
#define VL6180X_RESULT_HISTORY_BUFFER6               0x0058
#define VL6180X_RESULT_HISTORY_BUFFER7               0x0059
#define VL6180X_RESULT_HISTORY_BUFFER8               0x0060  // end of FIFO
#define VL6180X_RESULT_RANGE_VAL                     0x0062
#define VL6180X_RESULT_RANGE_RAW                     0x0064
#define VL6180X_RESULT_RANGE_RETURN_RATE             0x0066
#define VL6180X_RESULT_RANGE_REFERENCE_RATE          0x0068
#define VL6180X_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006C
#define VL6180X_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070
#define VL6180X_RESULT_RANGE_RETURN_AMB_COUNT        0x0074
#define VL6180X_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078
#define VL6180X_RESULT_RANGE_RETURN_CONV_TIME        0x007C
#define VL6180X_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080

#define VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD      0x010A
#define VL6180X_FIRMWARE_BOOTUP                      0x0119
#define VL6180X_FIRMWARE_RESULT_SCALER               0x0120
#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS             0x0212
#define VL6180X_INTERLEAVED_MODE_ENABLE              0x02A3

// I2C address  
#define VL6180X_DEFAULT_ADDRESS 0x29 //  0
#define VL6180X_0_ADDRESS       0x29 //  0
#define VL6180X_1_ADDRESS       0x31 //  1

enum VL6180XMode {
  contRangeMode = 0,
  contALSMode,
  interLeaveMode
};

enum ALSGain {  // define lower nibble of ALS gain register
  alsGain20 = 0,  // ALS gain of 20
  alsGain10,      // ALS gain of 10.32
  alsGain5,       // ALS gain of 5.21
  alsGain2_5,     // ALS gain of 2.60
  alsGain1_67,    // ALS gain of 1.72
  alsGain1_25,    // ALS gain of 1.28
  alsGain1_0,     // ALS gain of 1.01
  alsGain40       // ALS gain of 40
};


class VL6180X
{
  public: 
  VL6180X(I2Cdev* i2c_bus);
  void reset(uint8_t VL6180X_ADDRESS);
  void setI2CAddress(uint8_t VL6180X_ADDRESS, uint8_t newAddress);
  uint8_t getI2CAddress(uint8_t VL6180X_ADDRESS);
  uint8_t getDevID(uint8_t VL6180X_ADDRESS);
  void getDeviceInfo(uint8_t VL6180X_ADDRESS);
  float setalsGain(uint8_t _ALSGain);
  uint8_t getalsGain(uint8_t VL6180X_ADDRESS);
  void init(uint8_t VL6180X_ADDRESS, uint8_t VL6180XMode, uint8_t ALSGain);
  uint8_t getIntStatus(uint8_t VL6180X_ADDRESS);
  uint8_t getErrorStatus(uint8_t VL6180X_ADDRESS);
  uint8_t getRange(uint8_t VL6180X_ADDRESS);
  float getALS(uint8_t VL6180X_ADDRESS);
  void clearInt(uint8_t VL6180X_ADDRESS);
  
  private:
  // Register read variables
  I2Cdev* _i2c_bus;
  float realalsGain;
  };

#endif
