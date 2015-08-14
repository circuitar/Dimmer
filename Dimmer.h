/** @file
 * This is the Dimmer library to control the AC load power and also dimmerable lamps.
 *
 * Version 2.0 - Multi-instance objects.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#ifndef DIMMER_H
#define DIMMER_H

#include "Arduino.h"

/**
 * Maximum number of triacs that can be used. This is also the default value.
 */
#define MAX_TRIAC 10

/**
 * NanoShield_zeroCross pin settings.
 *
 * @param zeroCrossPin	Change this parameter to the pin your zero cross is attached.
 * @param zeroCrossInt	Change this parameter to the interruption correspondent to the pin of the zero cross. 
 * @see https://www.arduino.cc/en/Reference/attachInterrupt for more information.
 */
#define zeroCrossPin    2
#define zeroCrossInt    0

/**
 * Possible operation modes for the dimmer library.
 */
#define NORMAL_MODE     0
#define RAMP_MODE       1
#define COUNT_MODE      2

/**
 * Possible light state for the dimmer library.
 */
#define OFF    0
#define ON     1

/**
 * A ZeroCross Nanoshield or similar module to dimm AC lights.
 */
class DimmerControl
{    
  public:
  	/**
     * Constructor.
     *
     * Creates an object to access one Triac Nanoshield.
     * @param triacPin	pin select matching the jumper on the board. (D3, D5, etc)
     * @param value 	initial intensity in percentage of the dimm light.
     *					Minimum is 0 and maximum is 100. Defaut is 50%.
     * @param state 	initial state of the light. Possible states: ON or OFF. Default is OFF.
     * @see   begin()
     */ 
    DimmerControl(uint8_t triacPin, uint8_t value=50, bool state = OFF);

    /**
     * Initializes the module.
     *
     * Initializes ZeroCross and Timer 2 interrupts. Set the light according to initial settings.
     * @param mode 		operation mode to crontrol the light. 
     *					Possible modes:
     *					NORMAL_MODE: Uses timer to apply only a percentage of the AC power to the light.
     *					RAMP_MODE: Same as in normal mode, but it applies a ramp effect to the light.
     *					COUNT_MODE: Counts AC waves and applies full waves from time to time. @see resolution
	 * @param resolution 	IN COUNT MODE:
     *                          divided prescalar to define how many pulses to count.
	 *                          if resolution is 1 the mode counts 100 pulses, if resolution is 2
	 *                          the mode counts 50 pulses, and so on.
     *
     *                      IN RAMP MODE:
	 *                          Controlls the speed of the ramp when changing values.
     */
    bool begin(uint8_t mode = NORMAL_MODE, uint8_t resolution = 1);

    /**
     * Turns the light OFF. 
     */
    void off();

    /**
     * Turns the light ON. 
     */
    void on();

    /**
     * Toggles the light state. 
     */
    void toggle();

    /**
     * Gets the current value of the dimmed light.
     *
     * @return current light value.
     */
    bool getState();

    /**
     * Gets the current state of the dimmed light.
     *
     * @return current light state.
     */
    uint8_t getValue();

    /**
     * Sets the value and the state of the light.
     * @param value 	the value of the dimm light. Values from 0 to 100.
     * @param state 	the state of the light. ON or OFF.
     */
    void set(uint8_t value, uint8_t state = -1);

    void zeroCross();
    void triac();
    
  private:
    uint8_t operation_mode;
    uint8_t countResolution;
    uint8_t halfCycleCounter;
    uint8_t triacPin;
    uint8_t lampValue;
    uint8_t lampValue_ramp;
    bool lampState;
    unsigned int msCounter=0;
    unsigned int RampCounter=0;
};

void callZeroCross();

#endif