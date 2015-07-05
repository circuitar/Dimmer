/*
 This is the Dimmer library to control the AC load power and also dimmerable lamps
 
 Copyright (c) 2015 Circuitar
 This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Arduino.h"
#include <TimerOne.h>
#include "Dimmer.h"

//Time table to set triacs
const byte triac_time[] ={
    160, 146, 142, 138, 134, 132, 130, 128, 126, 124,
    122, 121, 120, 118, 117, 116, 114, 113, 112, 111,
    110, 109, 108, 107, 106, 105, 104, 103, 102, 101,
    100,  99,  98,  97,  96,  95,  94,  93,  92,  91,
    90,  89,  88,  87,  86,  85,  84,  83,  82,  81,
    80,  79,  78,  77,  76,  75,  74,  73,  72,  71,
    70,  69,  68,  67,  66,  65,  64,  63,  62,  61,
    60,  59,  58,  57,  56,  55,  54,  53,  52,  51,
    50,  48,  47,  46,  45,  44,  42,  41,  40,  38,
    36,  34,  32,  30,  28,  26,  24,  22,  18,  12, 6
};

static DimmerControl* Dimmers[MAX_TRIAC]; // Pointers to all registered dimmer objects
static uint8_t DimmerCount = 0;          // Number of registered dimmer objects
static bool zeroCrosAttached = FALSE;

//ZeroCross Interrupt
void callZeroCross(){
    for (uint8_t i = 0; i < DimmerCount; i++) {
        Dimmers[i]->zeroCross();
    }
}

//Class Constructor
DimmerControl::DimmerControl(uint8_t triacPin, uint8_t value, bool state, uint8_t resolution){
    this->triacPin = triacPin;
    this->lampValue = value;
    this->lampState = state;
    this->countResolution = resolution;
    zeroCrosAttached=false;
    triacAttached=false;
    triacs=0;

    //Register dimmer object
    Dimmers[DimmerCount++] = this;
}

//Set Zero cross
    

bool DimmerControl::begin(uint8_t mode){
    this->operation_mode = mode;
    halfCycleCounter = 0;
    pinMode(triacPin, OUTPUT); 
    if(!zeroCrosAttached){
        pinMode(zeroCrossPin, INPUT);
        attachInterrupt(zeroCrossInt, callZeroCross, RISING);
        zeroCrosAttached = TRUE; 
    }
}

void DimmerControl::off(){
    lampState=false;
}

void DimmerControl::on(){
    lampState=true;
}

void DimmerControl::toggle(){
    lampState=!lampState;
}

boolean DimmerControl::getState(){
    return lampState;
}

byte DimmerControl::getValue(){
    if (ramp_mode){
        return lampValue;
    }
    else{
        return lampValue_ramp;
    }
}

void DimmerControl::set(uint8_t value){
    lamp--;
    if (ramp_mode){
        lampValue=value;
    }
    else{
        lampValue_ramp=value;
    }
}

void DimmerControl::zeroCross(){
    if (countMode) {
        if (halfCycleCounter < lampValue_ramp / countResolution && lampState) {
            digitalWrite(triacPin, HIGH);
        } else {
            digitalWrite(triacPin, LOW);
        }

        if (++halfCycleCounter >= 100 / countResolution) {
            halfCycleCounter = 0;
        }
    } else {
        //Clear counter
        msCounter=0;
        //Turn-off triac
        digitalWrite(triacPin, LOW);   
    }
}

void DimmerControl::triac(){
    if (!countMode) {
        msCounter++;
        //With ramp mode
        if (ramp_mode && msCounter % 150 == 0 ) {
            if(lampValue_ramp > lampValue)
                lampValue_ramp--;
            else if(lampValue_ramp < lampValue)
                lampValue_ramp++;
        }
    
        if(lampValue_ramp > 0 && lampState){
            if(triac_time[ lampValue_ramp ] == msCounter){
                digitalWrite(triacPin, HIGH);
            }
        }
    }
}

ISR(TIMER2_OVF_vect) { //change this line
  // Process ISR for all configured dimmer lights at about 20KHz
    for (uint8_t i = 0; i < DimmerCount; i++) {
        Dimmers[i]->callTriac();
    }
}