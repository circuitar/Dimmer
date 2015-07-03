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
    
class DimmerControl{
    
public:
    DimmerControl();
    void attachZeroCross(byte pin, byte inter);
    void attachTriac(byte pin);
    void detachTriacs();
    int init();
    int initRamp();
    int initCount(byte resolution = 1);
    void off();
    void off(byte lamp);
    void on();
    void on(byte lamp);
    void toggle(byte lamp);
    boolean getState(byte lamp);
    byte getValue(byte lamp);
    void set(byte lamp, byte value);
    void zeroCross();
    void triac();

//private:
    int start();
    bool ramp_mode;
    bool countMode;
    byte countResolution;
    boolean zeroCrosAttached;
    boolean triacAttached;
    
    //Zero cross pin
    byte zeroCrossPin;
    
    //Zero cross interrupt
    byte zeroCrossInt;
    
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
    
    // Half cycle counter
    byte halfCycleCounter;
};

extern DimmerControl Dimmer;

void callZeroCross();
void callTriac();

#endif
