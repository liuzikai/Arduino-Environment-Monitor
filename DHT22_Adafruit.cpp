/**
 * A warpper class of Adafruit DHT lib to control DHT22 sensor.
 * Author: liuzikai
 * Date: June 2018
 */

#include "DHT22_Adafruit.h"

DHT my_dht22(MY_DHT_PIN, DHT22);

void My_DHT22::update() {
    temperature = my_dht22.readTemperature();
    humidity = my_dht22.readHumidity();
    dataValid = (!isnan(temperature) && !isnan(humidity));
}