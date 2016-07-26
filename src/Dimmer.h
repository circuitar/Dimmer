/** @file
 * This is a library to control the intensity of dimmable AC lamps or other AC loads using triacs.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#ifndef DIMMER_H
#define DIMMER_H

/**
 * Maximum number of triacs that can be used. Can be decreased to save RAM.
 */
#define DIMMER_MAX_TRIAC 10

/**
 * Timer to use for control of triac timing.
 */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define DIMMER_TIMER 4
#elif defined(__AVR_ATmega32U4__)
#define DIMMER_TIMER 3
#else
#define DIMMER_TIMER 2
#endif

/**
 * Zero cross circuit settings.
 *
 * @param DIMMER_ZERO_CROSS_PIN Change this parameter to the pin your zero cross circuit is attached.
 * @param DIMMER_ZERO_CROSS_INTERRUPT Change this parameter to the interrupt corresponding to the zero cross pin.
 *
 * @see https://www.arduino.cc/en/Reference/attachInterrupt for more information.
 */
#define DIMMER_ZERO_CROSS_PIN       2
#define DIMMER_ZERO_CROSS_INTERRUPT 0

/**
 * Possible operating modes for the dimmer library.
 */
#define DIMMER_NORMAL 0
#define DIMMER_RAMP   1
#define DIMMER_COUNT  2

/**
 * A dimmer channel.
 *
 * This object can control the power delivered to an AC load using a triac and a zero cross circuit.
 * A common zero cross circuit is shared across all instances, but each instance controls a separate
 * triac.
 */
class Dimmer {
  public:
    /**
     * Constructor.
     *
     * @param pin pin that activates the triac.
     * @param mode operating mode.
     *          Possible modes:
     *          NORMAL_MODE: Uses timer to apply only a percentage of the AC power to the lamp every half cycle.
     *          RAMP_MODE: Same as in normal mode, but it applies a ramp effect when changing levels. @see rampTime
     *          COUNT_MODE: Counts AC waves and applies full half cycles from time to time.
     * @param rampTime time it takes for the value to rise from 0% to 100% in RAMP_MODE, in seconds. Default 1.5. @see setRampTime().
     * @param freq AC frequency, in Hz. Supported values are 60Hz and 50Hz, use others at your own risk.
     *
     * @see begin()
     */
    Dimmer(uint8_t pin, uint8_t mode = DIMMER_NORMAL, double rampTime = 1.5, uint8_t freq = 60);

    /**
     * Initializes the module.
     *
     * Initializes zero cross circuit and Timer 2 interrupts. Set the lamp state and value according to initial settings.
     *
     * @param value initial intensity of the lamp, as a percentage. Minimum is 0, maximum is 100 and default is 0.
     * @param on initial lamp state. True if lamp is on or false if it's off. Lamp is on by default.
     */
    void begin(uint8_t value = 0, bool on = true);

    /**
     * Turns the lamp OFF.
     */
    void off();

    /**
     * Turns the lamp ON.
     */
    void on();

    /**
     * Toggles the lamp on/off state.
     */
    void toggle();

    /**
     * Gets the current value (intensity) of the lamp.
     *
     * @return current lamp value, from 0 to 100.
     */
    uint8_t getValue();

    /**
     * Gets the current state of the lamp.
     *
     * @return current lamp state. ON or OFF.
     */
    bool getState();

    /**
     * Sets the value of the lamp.
     *
     * @param value the value (intensity) of the lamp. Accepts values from 0 to 100.
     */
    void set(uint8_t value);

    /**
     * Sets the value and the state of the lamp.
     *
     * @param value value (intensity) of the lamp. Accepts values from 0 to 100.
     * @param on state of the lamp. True turns lamp on, false turns lamp off.
     */
    void set(uint8_t value, bool on);

    /**
     * Sets the mimimum acceptable power level. This is useful to control loads that cannot be
     * dimmed to a very low level, like dimmable LED or CFL lamps.
     *
     * @param value the minimum value (intensity) to use. Accepts values from 0 to 100.
     */
    void setMinimum(uint8_t value);

    /**
     * Sets tje time it takes for the value to rise from 0% to 100% in RAMP_MODE, in seconds.
     *
     * @param value the ramp time. Maximum is 2^16 / (2 * AC frequency)
     */
    void setRampTime(double rampTime);

  private:
    static bool started;
    uint8_t dimmerIndex;
    uint8_t triacPin;
    uint8_t operatingMode;
    bool lampState;
    uint8_t lampValue;
    uint8_t minValue;
    uint8_t rampStartValue;
    uint16_t rampCounter;
    uint16_t rampCycles;
    uint8_t acFreq;
    uint8_t pulseCount;
    uint8_t pulsesUsed;
    uint64_t pulsesHigh;
    uint64_t pulsesLow;

    void zeroCross();

    friend void callTriac();
    friend void callZeroCross();
};

#endif
