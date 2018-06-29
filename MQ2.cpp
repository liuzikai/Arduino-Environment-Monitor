/**
 * A simple class to control MQ2 sensor.
 * Author: liuzikai
 * Date: June 2018
 */

#include "MQ2.h"

MQ2::MQ2(int out_pin){
    OUT_PIN = out_pin;
}

void MQ2::begin(void) {
    pinMode(OUT_PIN, INPUT);
}

void MQ2::update(void) {
    outVoltage = analogRead(OUT_PIN) / 1024.0;
}