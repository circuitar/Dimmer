/*
 This is the Dimmer library to control the AC load power and also dimmerable lamps
 
 Copyright (c) 2015 Circuitar
 This software is released under the MIT license. See the attached LICENSE file for details.
 */

#ifndef DIMMER_H
#define DIMMER_H

#include "Arduino.h"
#include <TimerOne.h>

//Define Zero cross pin and interrupt here
#define zeroCrossPin    2
#define zeroCrossInt    0

#define MAX_TRIAC 10

 //Possible operation modes
#define NORMAL_MODE     0
#define RAMP_MODE       1
#define COUNT_MODE      2

 //Possible light state
#define OFF    0
#define ON     1

    
class DimmerControl{
    
public:
    DimmerControl(uint8_t triacPin, uint8_t value=50, bool state = ON, uint8_t resolution = 1);
    bool begin(uint8_t mode = NORMAL_MODE);
    void off();
    void on();
    void toggle();
    bool getState();
    byte getValue();
    void set(uint8_t value);
    void zeroCross();
    void triac();

//private:
    int start();
    uint8_t operation_mode;

    byte countResolution;
    bool zeroCrosAttached;
    
    //Triac pin vector
    byte triacPin;
    
    //Dimmer lamps value
    byte lampValue;
    
    //Temporary lamps value
    byte lampValue_ramp;
    
    //Virtual switch
    byte lampState;
    
    //50ms Counter
    unsigned int msCounter;
    
    // Half cycle counter
    byte halfCycleCounter;
};

void callZeroCross();

#endif