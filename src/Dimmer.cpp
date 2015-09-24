/*
 This is the Dimmer library to control AC loads, including dimmable lamps.
 
 Version 2.0 - Multi-instance objects.

 Copyright (c) 2015 Circuitar
 This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Arduino.h"
#include "Dimmer.h"

// Time table to set triacs
static int triacTime[] ={
  147, 142, 139, 136, 133, 131, 129, 127, 125, 124, 
  122, 121, 119, 118, 116, 115, 114, 113, 112, 111, 
  110, 108, 107, 106, 105, 104, 103, 102, 102, 101, 
  100,  99,  98,  97,  96,  95,  94,  93,  93,  92, 
   91,  90,  89,  88,  88,  87,  86,  85,  84,  83, 
   83,  82,  81,  80,  79,  78,  77,  77,  76,  75, 
   74,  73,  72,  71,  71,  70,  69,  68,  67,  66, 
   65,  64,  63,  62,  61,  60,  59,  58,  57,  56, 
   55,  54,  53,  51,  50,  49,  48,  46,  45,  43, 
   42,  40,  38,  36,  34,  31,  28,  24,  19,  0 
};

static Dimmer* dimmmers[MAX_TRIAC]; //  Pointers to all registered dimmer objects
static uint8_t dimmerCount = 0;     //  Number of registered dimmer objects
static bool started = false;

// ZeroCross Interrupt
void callZeroCross(){
  for (uint8_t i = 0; i < dimmerCount; i++) {
    dimmmers[i]->zeroCross();
  }
}

void callTriac(){
  //  Process ISR for all configured dimmer lights at about 20KHz
  for (uint8_t i = 0; i < dimmerCount; i++) {
    dimmmers[i]->triac();
  }
}

// Timer2 compare interruption
ISR(TIMER2_COMPA_vect) {
  callTriac();    
}


// Class Constructor
Dimmer::Dimmer(uint8_t triacPin, uint8_t mode, uint8_t value, bool state, uint16_t resolution){
  if (dimmerCount < MAX_TRIAC){
    if (mode == RAMP_MODE){
      this->lampValue = value;
      this->lampValueRamp = 0;
    }
    else{
      this->lampValueRamp = value;
    }

    this->triacPin = triacPin;
    this->operationMode = mode;
    this->lampState = state;
    this->countResolution = resolution;
    this->msCounter = 0;
    this->rampCounter = 0;

    // Register dimmer objects
    dimmmers[dimmerCount++] = this;
  }
}

bool Dimmer::begin(){
  pinMode(triacPin, OUTPUT);  

  if(!started){
    pinMode(zeroCrossPin, INPUT);
    attachInterrupt(zeroCrossInt, callZeroCross, RISING);
    started = true; 
  }

  if(operationMode != COUNT_MODE){
    // Setup Timer2 to fire every 50us
    TCCR2A = 0x02;    // CTC mode 
    TCCR2B = 0x02;    // prescaler=8
    TIMSK2 = 0x02;    // Timer2 Compare Match Interrupt Enable
    OCR2A = 99;       // Compare value
  }
}

void Dimmer::off(){
  lampState=false;
}

void Dimmer::on(){
  lampState=true;
}

void Dimmer::toggle(){
  lampState=!lampState;
}

bool Dimmer::getState(){
  return lampState;
}

uint8_t Dimmer::getValue(){
  if (operationMode == RAMP_MODE){
    return lampValue;
  }
  else{
    return lampValueRamp;
  }
}

void Dimmer::set(uint8_t value){
  if (operationMode == RAMP_MODE){
    lampValue=value;
  }
  else{
    lampValueRamp=value;
    if (operationMode == COUNT_MODE){
      pulses = 0;
      pulseCount = 0;
    }
  }
}

void Dimmer::set(uint8_t value, bool state){
  set(value);
  lampState = state;
}

void Dimmer::zeroCross(){
  if (operationMode == COUNT_MODE) {
    if (pulses >= MSB && pulseCount > 0){ // Check the MSB bit
      pulseCount--; 
    }

    pulses = pulses << 1;
    
    if (lampValueRamp > pulseCount * SCALE){
      digitalWrite(triacPin, HIGH);
        pulses++;
        pulseCount++;
    } 
    else {
      digitalWrite(triacPin, LOW);
    }
  } 
  // NORMAL OR RAMP MODE
  else {
    // Clear counter
    msCounter=0;
    // Turn-off triac
    digitalWrite(triacPin, LOW);   
  }
}

void Dimmer::triac(){
  if (operationMode != COUNT_MODE) {
    msCounter++;
    
    //With ramp mode
    if (operationMode == RAMP_MODE){
      rampCounter++;
      if(rampCounter % countResolution == 0 ) {
        rampCounter=0;
        if(lampValueRamp > lampValue){
          lampValueRamp--;
        }
        else if(lampValueRamp < lampValue){
          lampValueRamp++;
        }
      }
    }
  
    if(lampValueRamp > 0 && lampState){
      if(triacTime[lampValueRamp-1] <= msCounter){
        digitalWrite(triacPin, HIGH);
      }
    }
  }
}