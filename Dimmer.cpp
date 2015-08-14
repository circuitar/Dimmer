/*
 This is the Dimmer library to control AC loads, including dimmable lamps.
 
 Version 2.0 - Multi-instance objects.

 Copyright (c) 2015 Circuitar
 This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Arduino.h"
#include "Dimmer.h"

//Time table to set triacs
static uint8_t triac_time[] ={
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
static uint8_t DimmerCount = 0;           // Number of registered dimmer objects
static bool started = false;

//ZeroCross Interrupt
void callZeroCross(){
	for (uint8_t i = 0; i < DimmerCount; i++) {
		Dimmers[i]->zeroCross();
	}
}

//Class Constructor
DimmerControl::DimmerControl(uint8_t triacPin, uint8_t value, bool state){
	this->triacPin = triacPin;
	this->lampValue = value;
    this->lampState = state;

	//Register dimmer object
	Dimmers[DimmerCount++] = this;
}

bool DimmerControl::begin(uint8_t mode, uint8_t resolution){
    if (mode == RAMP_MODE && resolution == 1)
        resolution=200; //starting resolution for ramp mode
	this->operation_mode = mode;
    this->countResolution = resolution;
    pinMode(triacPin, OUTPUT);
    
	if(!started){
        halfCycleCounter = 0;
		pinMode(zeroCrossPin, INPUT);
		attachInterrupt(zeroCrossInt, callZeroCross, RISING);
		//Setup Timer2 to fire every 50ms
		TCCR2A = 0x02;    //CTC mode 
		TCCR2B = 0x02;    //prescaler=8
		TIMSK2 = 0x02;    //Timer2 Compare Match Interrupt Enable
		OCR2A = 49;		  //Compare value
		started = true; 
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

bool DimmerControl::getState(){
	return lampState;
}

uint8_t DimmerControl::getValue(){
	if (operation_mode == RAMP_MODE){
		return lampValue;
	}
	else{
		return lampValue_ramp;
	}
}

void DimmerControl::set(uint8_t value, uint8_t state){
	if (operation_mode == RAMP_MODE){
		lampValue=value;
	}
	else{
		lampValue_ramp=value;
	}

	if (state != -1){
		lampState = (bool) state;
	}
}

void DimmerControl::zeroCross(){
	if (operation_mode == COUNT_MODE) {
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
	if (operation_mode != COUNT_MODE) {
		msCounter++;
        RampCounter++;
		//With ramp mode
		if (operation_mode == RAMP_MODE && RampCounter % countResolution == 0 ) {
            RampCounter=0;
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

//Timer2 compare interruption
ISR(TIMER2_COMPA_vect) {
  // Process ISR for all configured dimmer lights at about 20KHz
	for (uint8_t i = 0; i < DimmerCount; i++) {
		Dimmers[i]->triac();
	}
}