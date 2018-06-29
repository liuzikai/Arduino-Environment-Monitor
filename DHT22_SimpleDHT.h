/**
 * A warpper class of SimpleDHT to control DHT22 sensor.
 * Author: liuzikai
 * Date: June 2018
 */

#ifndef ENVIRONMENT_MONITOR_DHT22_SIMPLEDHT_H_
#define ENVIRONMENT_MONITOR_DHT22_SIMPLEDHT_H_

#include <SimpleDHT.h>

class My_DHT22 {
    
private:

    int data_pin;

    SimpleDHT22 dht22;

public:

    My_DHT22(int pin) {
        data_pin = pin;
    }

    float temperature;
    float humidity;
    int error;

    void update() {
        error = dht22.read2(data_pin, &temperature, &humidity, NULL);
    }

};

#endif // ENVIRONMENT_MONITOR_DHT22_SIMPLEDHT_H_