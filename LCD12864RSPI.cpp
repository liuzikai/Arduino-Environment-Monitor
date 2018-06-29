/***********************************************************
File name: Adeept_LCD12864RSPI.cpp
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

#include "LCD12864RSPI.h"


extern "C"
{
#include "Arduino.h"
#include <inttypes.h>
#include <stdio.h>  //not needed yet
#include <string.h> //needed for strlen()
#include <avr/pgmspace.h>
}

LCD12864RSPI::LCD12864RSPI(int latch_pin, int clock_pin, int data_pin) {
  LATCH_PIN = latch_pin;
  CLOCK_PIN = clock_pin;
  DATA_PIN = data_pin;
}

inline void LCD12864RSPI::writeByte(int dat)
{
    digitalWrite(LATCH_PIN, HIGH);
    delayMicroseconds(DELAY_TIME);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, dat);
    digitalWrite(LATCH_PIN, LOW);
}

inline void LCD12864RSPI::writeCommand(int cmd)
{
   int H_data,L_data;
   H_data = cmd;
   H_data &= 0xf0;           //Mask the lower 4 bits of data
   L_data = cmd;             //xxxx0000format
   L_data &= 0x0f;           //Shield high 4 bits of data
   L_data <<= 4;             //xxxx0000format
   writeByte(0xf8);          //RS=0��Written instructions;
   writeByte(H_data);
   writeByte(L_data);
}

inline void LCD12864RSPI::writeData(int cmd)
{
   int H_data,L_data;
   H_data = cmd;
   H_data &= 0xf0;           //Mask the lower 4 bits of data
   L_data = cmd;             //xxxx0000format
   L_data &= 0x0f;           //Shield high 4 bits of data
   L_data <<= 4;             //xxxx0000format
   writeByte(0xfa);          //RS=1��The data is written
   writeByte(H_data);
   writeByte(L_data);
}

void LCD12864RSPI::init()
{
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(LATCH_PIN, LOW);
    delayMicroseconds(DELAY_TIME);

    writeCommand(0x30);        //Function Set the control word
    writeCommand(0x0c);        //Displays the switch control word
    writeCommand(0x01);        //Clear the screen control word
    writeCommand(0x06);        //Enter the setpoint control word
}


void LCD12864RSPI::clear(void)
{
    writeCommand(0x30);//
    writeCommand(0x01);//Clear the display
}


void LCD12864RSPI::displayString(int X, int Y, char* ptr, int dat)
{
  switch(X)
   {
     case 0:  Y |= 0x80 ;break;
     case 1:  Y |= 0x90; break;
     case 2:  Y |= 0x88; break;
     case 3:  Y |= 0x98; break;
     default: break;
   }
  writeCommand(Y); // Positioning Displays the start address

  for(int i = 0; i < dat; i++) {
      writeData(ptr[i]);//Note that the Chinese character code value, two consecutive yards that a Chinese character
    }
}

void LCD12864RSPI::displayString_P(int X, int Y, const char* ptr, int dat)
{
  switch(X)
   {
     case 0:  Y |= 0x80 ;break;
     case 1:  Y |= 0x90; break;
     case 2:  Y |= 0x88; break;
     case 3:  Y |= 0x98; break;
     default: break;
   }
  writeCommand(Y); // Positioning Displays the start address

  for(int i = 0; i < dat; i++) {
      writeData(pgm_read_byte_near(ptr + i));//Note that the Chinese character code value, two consecutive yards that a Chinese character
    }
}


void LCD12864RSPI::displaySig(int M, int N, int sig)
{
  switch(M)
   {
     case 0:  N |= 0x80; break;
     case 1:  N |= 0x90; break;
     case 2:  N |= 0x88; break;
     case 3:  N |= 0x98; break;
     default: break;
   }
  writeCommand(N); // Positioning Displays the start address
  writeData(sig);  //Outputs a single character
 }




void LCD12864RSPI::drawFullScreen(char* p)
{
      int ygroup, x, y, i;
      int temp;
      int tmp;

      for(ygroup = 0; ygroup < 64; ygroup++) //Writing the half-image portion of the liquid crystal
        {                              //Write coordinates
           if(ygroup < 32)
            {
             x = 0x80;
             y = ygroup + 0x80;
            }
           else
            {
              x = 0x88;
              y = ygroup - 32 + 0x80;
            }
           writeCommand(0x34);        //Write extended instruction command
           writeCommand(y);           //Writes y-axis coordinates
           writeCommand(x);           //Writes x-axis coordinates
           writeCommand(0x30);        //Write basic command command
           tmp = ygroup * 16;
           for(i = 0; i < 16; i++)
		 {
		    temp = p[tmp++];
		    writeData(temp);
               }
          }
        writeCommand(0x34);        //Write extended instruction command
        writeCommand(0x36);        //Display the image
}
