/***********************************************************
File name: Adeept_LCD12864RSPI.h
Description: Demo LCD12864 spi
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2017/01/04 
***********************************************************/

/**
 * 
 * Revised by liuzikai
 * Date: 2018/06/15
 * Change naming.
 * Add displayString_P() to support progmem.
 */

#ifndef ENVIRONMENT_MONITOR_ADEEPT_LCD12864RSPI_H_
#define ENVIRONMENT_MONITOR_ADEEPT_LCD12864RSPI_H_

#include <Arduino.h> 
#include <avr/pgmspace.h>
#include <inttypes.h>

#define DELAY_TIME 80

class LCD12864RSPI {

private:

    int LATCH_PIN;
    int CLOCK_PIN;
    int DATA_PIN;

public:

    void init(void);

    inline void writeByte(int dat);
    inline void writeCommand(int cmd);
    inline void writeData(int cmd);


    void clear(void);
    void displayString(int X, int Y, char *ptr, int dat);
    void displayString_P(int X, int Y, const char *ptr, int dat);
    void displaySig(int M, int N, int sig);
    void drawFullScreen(char *p);

    LCD12864RSPI(int latch_pin, int clock_pin, int data_pin);

};
#endif // ENVIRONMENT_MONITOR_ADEEPT_LCD12864RSPI_H_
