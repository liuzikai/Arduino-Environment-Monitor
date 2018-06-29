/**
 * A simple class to control photosensor.
 * Author: liuzikai
 * Date: June 2018
 */

#include "Photosensor.h"

Photosensor::Photosensor(int out_pin){
    OUT_PIN = out_pin;
}

void Photosensor::begin(void) {
    pinMode(OUT_PIN, INPUT);
}

void Photosensor::update(void) {
    outVoltage = analogRead(OUT_PIN) / 1024.0;
}