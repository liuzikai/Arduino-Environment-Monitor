/*
  Adeept BMP180 Module library V1.0
  2017 Copyright (c) Adeept Technology Inc.  All right reserved.
  Author: TOM
*/

/**
 * Revised by liuzikai 
 */

#include "Adeept_BMP180.h"

Adeept_BMP180::Adeept_BMP180(int BMP180_ADDRESS)
{
   _BMP180_ADDRESS = BMP180_ADDRESS;
}

/*** Basic IO operations ***/

// Read 1 byte from the BMP180 at 'address'
unsigned char Adeept_BMP180::read(int address)
{
  unsigned char data;

  Wire.beginTransmission(_BMP180_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(_BMP180_ADDRESS, 1);
  while (!Wire.available());

  return Wire.read();
}

// Read 2 bytes from the BMP180
// First byte will be from 'address'
// Second byte will be from 'address'+1
int Adeept_BMP180::readInt(int address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(_BMP180_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(_BMP180_ADDRESS, 2);
  while (Wire.available() < 2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb << 8 | lsb;
}

void Adeept_BMP180::writeRegister(int deviceAddress, byte address, byte val) {
  Wire.beginTransmission(deviceAddress); // start transmission to device
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int Adeept_BMP180::readRegister(int deviceAddress, byte address) {

  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while (!Wire.available()) {
    // waiting
  }

  v = Wire.read();
  return v;
}

/*** Data Decode Operations ***/

// Calculate temperature in deg C
void Adeept_BMP180::readTemperature(void) {
  long x1, x2;
  // Read the uncompensated temperature value
  unsigned int ut;
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(_BMP180_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = readInt(0xF6);

  x1 = (((long)ut - (long)ac6) * (long)ac5) >> 15;
  x2 = ((long)mc << 11) / (x1 + md);
  b5 = x1 + x2;

  float temp = ((b5 + 8) >> 4);
  temp = temp / 10;

  temperature = temp;
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp180GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
void Adeept_BMP180::readPressure(void) {
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  // Read the uncompensated pressure value
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(_BMP180_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS << 6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3 << OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  msb = read(0xF6);
  lsb = read(0xF7);
  xlsb = read(0xF8);

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8 - OSS);

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6) >> 12) >> 11;
  x2 = (ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;

  // Calculate B4
  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

  b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
  if (b7 < 0x80000000)
    p = (b7 << 1) / b4;
  else
    p = (b7 / b4) << 1;

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  p += (x1 + x2 + 3791) >> 4;

  pressure = p;
}

void Adeept_BMP180::calcAltitude() {

  float A = pressure / 101325;
  float B = 1 / 5.25588;
  float C = pow(A, B);
  C = 1 - C;
  C = C / 0.0000225577;

  altitude = C;
}

/*** Interface ***/

void Adeept_BMP180::update(void) {
  readTemperature();
  readPressure();
  calcAltitude();
}

void Adeept_BMP180::begin(void)
{
  Wire.begin();

  // Calibration
  //  Stores all of the bmp180's calibration values into global variables
  //  Calibration values are required to calculate temp and pressure
  //  This function should be called at the beginning of the program
  ac1 = readInt(0xAA);
  ac2 = readInt(0xAC);
  ac3 = readInt(0xAE);
  ac4 = readInt(0xB0);
  ac5 = readInt(0xB2);
  ac6 = readInt(0xB4);
  b1 = readInt(0xB6);
  b2 = readInt(0xB8);
  mb = readInt(0xBA);
  mc = readInt(0xBC);
  md = readInt(0xBE);
}
