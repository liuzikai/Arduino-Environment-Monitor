/*
  Adeept BMP180 Module library V1.0
  2017 Copyright (c) Adeept Technology Inc.  All right reserved.
  Author: TOM
*/

/**
 * Revised by liuzikai 
 */

#ifndef ENVIRONMENT_MONITOR_ADEEPT_BMP180_H_
#define ENVIRONMENT_MONITOR_ADEEPT_BMP180_H_

#include <Arduino.h>
#include <Wire.h>

class Adeept_BMP180
{
  private:
  
  const unsigned char OSS = 0;  // Oversampling Setting
  int _BMP180_ADDRESS;

  // Calibration values
  int ac1;
  int ac2;
  int ac3;
  unsigned int ac4;
  unsigned int ac5;
  unsigned int ac6;
  int b1;
  int b2;
  int mb;
  int mc;
  int md;
  // b5 is calculated in bmp180GetTemperature(...), this variable is also used in bmp180GetPressure(...)
  // so ...Temperature(...) must be called before ...Pressure(...).
  long b5;

  unsigned char read(int address);
  int readInt(int address);

  void writeRegister(int deviceAddress, byte address, byte val);
  int readRegister(int deviceAddress, byte address);

  void readTemperature(void);
  void readPressure(void);
  void calcAltitude();

public:

  Adeept_BMP180(int BMP180_ADDRESS);

  float temperature;
  float pressure;
  float altitude;

  void update(void);
  void begin(void);  // Initialize adressing and calibrate
  
};

#endif // ENVIRONMENT_MONITOR_ADEEPT_BMP180_H_