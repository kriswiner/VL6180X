/* 06/02/2020 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This sketch uses SDA/SCL on pins 20/21, respectively, and it uses the Ladybug STM32L432KC Breakout Board.
 The VL6180X is a time-of-flight ranging and ambient light sensor.
 
 Library may be used freely and without limit with attribution.
 
*/

#include "VL6180X.h"

  VL6180X::VL6180X(I2Cdev* i2c_bus)
  {
  _i2c_bus = i2c_bus;
  }


  void  VL6180X::setI2CAddress(uint8_t VL6180X_ADDRESS, uint8_t newAddress)
  {
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_I2C_SLAVE_DEVICE_ADDRESS, newAddress);
  }


  uint8_t  VL6180X::getI2CAddress(uint8_t VL6180X_ADDRESS)
  {
    uint8_t temp =_i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_I2C_SLAVE_DEVICE_ADDRESS);
    return temp;
  }


  uint8_t  VL6180X::getDevID(uint8_t VL6180X_ADDRESS)
  {
    uint8_t temp =_i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_WHO_AM_I);
    return temp;
  }


  void VL6180X::getDeviceInfo(uint8_t VL6180X_ADDRESS)
  {
  // Get info about the specific device
  uint8_t majmodrev = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODEL_REV_MAJOR);    // Read MAJOR_MODEL_REVISION register for VL6180X
  Serial.print("Major model revision "); Serial.println((majmodrev & 0x07));  
  uint8_t minmodrev = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODEL_REV_MINOR);    // Read MINOR_MODEL_REVISION register for VL6180X
  Serial.print("Minor model revision "); Serial.println((minmodrev & 0x07));  
  uint8_t majmodurev = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODULE_REV_MAJOR);  // Read MAJOR_MODULE_REVISION register for VL6180X
  Serial.print("Major module revision "); Serial.println((majmodurev & 0x07));  
  uint8_t minmodurev = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODULE_REV_MINOR);  // Read MINOR_MODULE_REVISION register for VL6180X
  Serial.print("Minor module revision "); Serial.println((minmodurev & 0x07));  
  uint8_t dateh = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_DATE_HI);  // Read IDENTIFICATION_DATE_HI register for VL6180X
  Serial.print("Manufactured in 201"); Serial.print((dateh & 0xF0) >> 4 ); Serial.print(", month ");  Serial.println(dateh & 0x0F); 
  uint8_t datel = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_DATE_LO);  // Read IDENTIFICATION_DATE_LO register for VL6180X
  Serial.print("Manufactured on day "); Serial.print((datel & 0xF8) >> 3 ); Serial.print(", manufacturing phase ");  Serial.println(datel & 0x07); 
  uint8_t timeh = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_TIME_HI);  // Read IDENTIFICATION_TIME register for VL6180X
  uint8_t timel = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_TIME_LO);  // Read IDENTIFICATION_TIME register for VL6180X
  uint16_t mtime = ((uint16_t)timeh << 8 | timel);
  Serial.print("Manufactured "); Serial.print(2*mtime/3600); Serial.println(" hours after midnight "); 
  }


  float VL6180X::setalsGain(uint8_t ALSGain) {
    float realalsGain = 0;
  switch (ALSGain)
  {
   // Possible ALS gains
    case alsGain20:
          realalsGain = 20.0f; // get actual ALS gain from nominal index
          break;
    case alsGain10:
          realalsGain = 10.32f;  
          break;
    case alsGain5:
          realalsGain = 5.21f;  
          break;
    case alsGain2_5:
          realalsGain = 2.60f;
          break;
    case alsGain1_67:
          realalsGain = 1.72f;  
          break;
    case alsGain1_25:
          realalsGain = 1.28f;  
          break;
    case alsGain1_0:
          realalsGain = 1.01f;  
          break;
    case alsGain40:
          realalsGain = 40.0f;  
          break;
  }
   
  return realalsGain;
}


uint8_t VL6180X::getalsGain(uint8_t VL6180X_ADDRESS)
{
  uint8_t temp = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN);
  return temp;
}


void VL6180X::init(uint8_t VL6180X_ADDRESS, uint8_t VL6180XMode, uint8_t ALSGain) {
  
  uint8_t reset = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET);  // read fresh_out_of_reset bit
  
  if(reset == 1) 
  {  //  if fresh_out_of_reset bit set, then device has been freshly initialized
  
// SR03 settings AN4545 24/27 DocID026571 Rev 19 SR03 settings
// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
// Below are the recommended settings required to be loaded onto the VL6180X during the
// initialisation of the device (see Section 1.3).
// Mandatory : private registers
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0207, 0x01);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0208, 0x01);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0096, 0x00);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0097, 0xFD);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00e3, 0x00);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00e4, 0x04);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00e5, 0x02);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00e6, 0x01);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00e7, 0x03);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00f5, 0x02);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00d9, 0x05);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00db, 0xce);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00dc, 0x03);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00dd, 0xf8);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x009f, 0x00);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00a3, 0x3c);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00b7, 0x00);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00bb, 0x3c);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00b2, 0x09);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00ca, 0x09);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0198, 0x01);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x01b0, 0x17);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x01ad, 0x00);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x00ff, 0x05);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0100, 0x05);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0199, 0x05);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x01a6, 0x1b);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x01ac, 0x3e);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x01a7, 0x1f);
    _i2c_bus->writeByte16(VL6180X_ADDRESS, 0x0030, 0x00);
    
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00);  // clear fresh_out_of_reset bit

// Configure range measurement for low power
// Specify range measurement interval in units of 10 ms from 0 (= 10 ms) - 254 (= 2.55 s)
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09); // 100 ms interval in steps of 10 ms
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // sets number of range measurements after which autocalibrate is performed
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform temperature calibration of the ranging sensor

// Set Early Convergence Estimate for lower power consumption
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32); // set max convergence time to 50 ms (steps of 1 ms)
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
// This ECE is calculated as follows:
// [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
// This is ~123 ms for 50 ms max convergence time and 80% below threshold
// This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x7B); // set early convergence estimate to 5%

// Configure ALS
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A);   // set to 100 ms
// Following is a 16-bit register with the first MSByte reserved
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x63);        // set ALS integration time to 100 ms in steps of 1 ms

// The internal readout averaging sample period can be adjusted from 0 to 255. Increasing the sampling 
// period decreases noise but also reduces the effective max convergence time and increases power consumption:
// Effective max convergence time = max convergence time - readout averaging period (see
// Section 2.5: Range timing). Each unit sample period corresponds to around 64.5 μs additional
// processing time. The recommended setting is 48 which equates to around 4.3 ms
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30);  // compromise between low noise and increased execution time
// Gain can be 0 = 20, 1 = 10, 2 = 5, 3 = 2.5, 4 = 1.67, 5 = 1.25, 6 = 1.0 and 7 = 40
// These are value possible for the lower nibble. The upper nibble must be 4
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN, 0x04 | ALSGain);   // Sets light and dark gain (don't change upper nibble)
// Scalar (1 - 32, bits 4:0) to multiply raw ALS count for additonal gain, if necessary
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_FIRMWARE_RESULT_SCALER, 0x01);

// Configure the interrupts
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO0, 0x00);                // set up GPIO 0 (set to high impedence for now)
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO1, 0x30);                // set up GPIO 1 active HIGH interrupt
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24);     // enable sample ready interrupt

    // enable continuous range mode
    if(VL6180XMode == contRangeMode) {
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_START, 0x03);  // start auto range mode 
    }
    
    // enable continuous ALS mode
    if(VL6180XMode == contALSMode) {
    // Configure ALS
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x32);   // set to 100 ms
    // Following is a 16-bit register with the first MSByte reserved
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x32);        // set ALS integration time to 50 ms in steps of 1 ms
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_START, 0x03);                     // start auto range mode
    }

    if(VL6180XMode == interLeaveMode) {
// Configure ALS for interleaved mode at 10 Hz
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A);       // set to 100 ms
// Following is a 16-bit register with the first MSByte reserved
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x32);          // set ALS integration time to 50 ms in steps of 1 ms
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x1E);        // set max convergence time to 30 ms
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
// This ECE is calculated as follows:
// [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
// This is ~72 ms for 30 ms max convergence time and 80% below threshold
// This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x48); // set early convergence estimate to 5%
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_INTERLEAVED_MODE_ENABLE, 0x01);  // eanble interleave mode
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSALS_START, 0x03);  // start continuous als measurement mode
// range read automatically performed immediately after each ALS measurement

// Clear reset bit
    _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00);        // reset fresh_out_of_reset bit to zero  
    }

  }

}
 

uint8_t VL6180X::getIntStatus(uint8_t VL6180X_ADDRESS)
{
      uint8_t temp = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);  // read new sample data ready status register
      return temp;
}


uint8_t VL6180X::getErrorStatus(uint8_t VL6180X_ADDRESS)
{
      uint8_t temp = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_STATUS);  // read new sample data ready status register
      return temp;
}


uint8_t VL6180X::getRange(uint8_t VL6180X_ADDRESS)
{
      uint8_t temp = _i2c_bus->readByte16(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_VAL);  // read new sample data ready status register
      return temp;
}


void VL6180X::clearInt(uint8_t VL6180X_ADDRESS)
{
  _i2c_bus->writeByte16(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
}


 float VL6180X::getALS(uint8_t VL6180X_ADDRESS)
{   
  uint8_t rawData[2] = {0, 0};
  _i2c_bus->readBytes16(VL6180X_ADDRESS, VL6180X_RESULT_ALS_VAL, 2, &rawData[0]); // two-byte als data
  uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]);       // get 16-bit als raw value
  float als = 0.32f * ((float) alsraw / realalsGain) * (100.0f/100.0f);           // convert to absolute lux
  return als;
}
