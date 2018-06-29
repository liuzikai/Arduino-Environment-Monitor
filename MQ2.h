/**
 * A simple class to control MQ2 sensor.
 * Author: liuzikai
 * Date: June 2018
 */

#ifndef ENVIRONMENT_MONITOR_MQ2_H_
#define ENVIRONMENT_MONITOR_MQ2_H_

#include <Arduino.h>

class MQ2 {

    int OUT_PIN;

public:

    MQ2(int out_pin);
    void begin();
    float outVoltage;
    void update(void);
};


#endif // ENVIRONMENT_MONITOR_MQ2_H_