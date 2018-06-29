/**
 * A warpper class of Adafruit DHT lib to control DHT22 sensor.
 * Author: liuzikai
 * Date: June 2018
 */

#ifndef ENVIRONMENT_MONITOR_DHT22_ADAFRUIT_H_
#define ENVIRONMENT_MONITOR_DHT22_ADAFRUIT_H_

#include <DHT.h>

#define MY_DHT_PIN 2

class My_DHT22 {
    
public:

    float temperature;
    float humidity;
    bool dataValid;

    void update();

};

#endif // ENVIRONMENT_MONITOR_DHT22_ADAFRUIT_H_