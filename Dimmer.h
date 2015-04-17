/*
 This is the Dimmer library to control the AC load power and also dimmerable lamps
 
 Copyright (c) 2015 Circuitar
 This software is released under the MIT license. See the attached LICENSE file for details.
 */

#ifndef DIMMER_H
#define DIMMER_H

#include "Arduino.h"
#include <TimerOne.h>

#define MAX_TRIAC 10

#ifndef dimmerObject
#define dimmerObject    dimmer1
#endif

class Dimmer{
    
public:
    Dimmer();
    void attachZeroCross(int zc_pin, int inter);
    void attachTriac(byte pin);
    int init();
    int initRamp();
    void off();
    void off(byte lamp);
    void on();
    void on(byte lamp);
    void toggle(byte lamp);
    void set(byte lamp, byte value);
    void zeroCross();
    void triac();

//private:
    int start();
    boolean ramp_mode;
    boolean zeroCrosAttached;
    boolean triacAttached;
    
    //Triac pin vector
    byte triacPins[MAX_TRIAC];
    
    //Triac pin counter
    byte triacs;
    
    //Dimmer lamps value
    byte lampValue[MAX_TRIAC];
    
    //Temporary lamps value
    byte lampValue_ramp[MAX_TRIAC];
    
    //Virtual switch
    byte lampSwitch[MAX_TRIAC];
    
    //50ms Counter
    unsigned int msCounter;
};

extern Dimmer dimmerObject;

void callZeroCross();
void callTriac();

#endif
