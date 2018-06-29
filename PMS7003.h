/**
 * Class to handle Plantower PMS7003 sensor.
 * Author: liuzikai
 * Date: June 2018
 */ 

#ifndef ENVIRONMENT_MONITOR_PMS7003_H_
#define ENVIRONMENT_MONITOR_PMS7003_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 6
#define TX_PIN 10

class PMS7003 {

private:

    uint16_t inputChecksum = 0;
    uint16_t checksum = 0;

    // Be careful of the data type
    int inputHigh = 0;
    int inputLow = 0;

    uint8_t readUInt8(void);
    uint16_t readUInt16(void);

public:
    
    bool dataAvailable(void);

    uint16_t pm1_0_cf1;
    uint16_t pm2_5_cf1;
    uint16_t pm10_0_cf1;
    uint16_t pm1_0_amb;
    uint16_t pm2_5_amb;
    uint16_t pm10_0_amb;
    uint16_t pm0_3_raw;
    uint16_t pm0_5_raw;
    uint16_t pm1_0_raw;
    uint16_t pm2_5_raw;
    uint16_t pm5_0_raw;
    uint16_t pm10_0_raw;
    uint8_t  version;
    uint8_t  errorCode;
    bool dataValid = false;

    void begin(void);
    //Require call interval from 200 to 800 ms
    bool update(void);
};

#endif // ENVIRONMENT_MONITOR_PMS7003_H_
