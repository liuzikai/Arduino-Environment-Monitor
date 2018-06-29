/**
 * A simple class to control photosensor.
 * Author: liuzikai
 * Date: June 2018
 */

#ifndef ENVIRONMENT_MONITOR_PHOTOSENSOR_H_
#define ENVIRONMENT_MONITOR_PHOTOSENSOR_H_

#include <Arduino.h>

class Photosensor {

    int OUT_PIN;

public:

    Photosensor(int out_pin);
    void begin();
    float outVoltage;
    void update(void);
};


#endif // ENVIRONMENT_MONITOR_PHOTOSENSOR_H_