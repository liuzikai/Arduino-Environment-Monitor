/**
 * Class to handle Plantower PMS7003 sensor.
 * Author: liuzikai
 * Date: June 2018
 */ 

#include "PMS7003.h"

SoftwareSerial pmsSerial(RX_PIN, TX_PIN);

// Wait until the information is fully received
bool PMS7003::dataAvailable(void) {
    return (pmsSerial.available() >= 32);
}

/* Helper function to retrive data from serial and add checkSum */

uint8_t PMS7003::readUInt8(void) {
    inputLow = pmsSerial.read();
    inputChecksum += inputLow;
    return inputLow;
}

uint16_t PMS7003::readUInt16(void) {
    inputHigh = pmsSerial.read();
    inputLow = pmsSerial.read();
    inputChecksum += inputHigh + inputLow;
    return inputLow + (inputHigh << 8);
}

void PMS7003::begin(void) {
    pmsSerial.begin(9600);
}

bool PMS7003::update(void) {
    
    inputChecksum = 0;
    
    // Data header
    if (readUInt8() != 0x42) return false;
    if (readUInt8() != 0x4D) return false;

    // Data lenth
    if (readUInt8() != 0x00) return false;
    if (readUInt8()!= 0x1c) return false;

    pm1_0_cf1 = readUInt16();
    pm2_5_cf1 = readUInt16();
    pm10_0_cf1 = readUInt16();

    pm1_0_amb = readUInt16();
    pm2_5_amb = readUInt16();
    pm10_0_amb = readUInt16();

    pm0_3_raw = readUInt16();
    pm0_5_raw = readUInt16();
    pm1_0_raw = readUInt16();
    pm2_5_raw = readUInt16();
    pm5_0_raw = readUInt16();
    pm10_0_raw = readUInt16();

    version = readUInt8();
    errorCode = readUInt8();

    inputHigh = pmsSerial.read();
    inputLow = pmsSerial.read();
    checksum = inputLow + (inputHigh << 8);
    dataValid = (checksum == inputChecksum);

    return true;
}
