/** @file
 * This is a library to control the intensity of dimmable AC lamps or other AC loads using triacs.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Arduino.h"
#include "Dimmer.h"

// Timer configuration macros
#define _TCNT(X) TCNT ## X
#define TCNT(X) _TCNT(X)
#define _TCCRxA(X) TCCR ## X ## A
#define TCCRxA(X) _TCCRxA(X)
#define _TCCRxB(X) TCCR ## X ## B
#define TCCRxB(X) _TCCRxB(X)
#define _TIMSKx(X) TIMSK ## X
#define TIMSKx(X) _TIMSKx(X)
#define _OCRxA(X) OCR ## X ## A
#define OCRxA(X) _OCRxA(X)
#define _TIMER_COMPA_VECTOR(X) TIMER ## X ## _COMPA_vect
#define TIMER_COMPA_VECTOR(X) _TIMER_COMPA_VECTOR(X)

#if DIMMER_TIMER == 2

// 8-bit timer
#define TCCRxA_VALUE 0x02 // CTC mode
#define TCCRxB_VALUE 0x02 // Prescaler = 8 => Cycle = 0.5us @ 16MHz

#elif (DIMMER_TIMER == 1 || DIMMER_TIMER == 3 || DIMMER_TIMER == 4 || DIMMER_TIMER == 5)

// 16-bit timer ()
#define TCCRxA_VALUE 0x00 // CTC mode
#define TCCRxB_VALUE 0x0A // CTC mode, Prescaler = 8 => Cycle = 0.5us @ 16MHz

#endif

// Time table to set triacs (50us tick)
//   This is the number of ticks after zero crossing that we should wait before activating the
//   triac, where the array index is the applied power from 1 to 100, minus one. This array is
//   calculated based on the equation of the effective power applied to a resistive load over
//   time, assuming a 60HZ sinusoidal wave.
static uint8_t triacTime[] = {
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

// Dimmer registry
static Dimmer* dimmmers[DIMMER_MAX_TRIAC]; // Pointers to all registered dimmer objects
static uint8_t dimmerCount = 0;            // Number of registered dimmer objects

static uint8_t tmrCount = 0;

bool Dimmer::started = false;
bool Dimmer::timerStarted = false;

// Zero cross interrupt
void callZeroCross() {
  // Reset Timer 2 and clear interrupt counter
  TCNT(DIMMER_TIMER) = 0;
  tmrCount = 0;

  // Process each registered dimmer object
  for (uint8_t i = 0; i < dimmerCount; i++) {
    dimmmers[i]->zeroCross();
  }
}

void callTriac() {
  // Increment Timer 2 interrupt counter
  if (tmrCount < 255) {
    tmrCount++;
  }

  //  Process ISR for all configured dimmer lights at about 20KHz
  for (uint8_t i = 0; i < dimmerCount; i++) {
    dimmmers[i]->triac();
  }
}

// Timer2 compare interruption
ISR(TIMER_COMPA_VECTOR(DIMMER_TIMER)) {
  callTriac();
}

// Constructor
Dimmer::Dimmer(uint8_t pin, uint8_t mode, double rampTime, uint8_t freq) :
        triacPin(pin),
        triacPinMask(digitalPinToBitMask(pin)),
        triacPinPort(portOutputRegister(digitalPinToPort(pin))),
        operatingMode(mode),
        currentTriacTime(255),
        lampState(false),
        lampValue(0),
        rampStartValue(0),
        rampCounter(1),
        rampCycles(1),
        pulseCount(0),
        pulses(0),
        acFreq(freq) {
  if (dimmerCount < DIMMER_MAX_TRIAC) {
    // Register dimmer object being created
    dimmmers[dimmerCount++] = this;
  }

  if (mode == DIMMER_RAMP) {
    rampTime = rampTime * 2 * freq;
    rampCycles = rampTime > 0xFFFF ? 0xFFFF : rampTime;
    rampCounter = rampCycles;
  }
}

void Dimmer::begin(uint8_t value, bool on) {
  // Initialize lamp state and value
  set(value, on);

  // Initialize triac pin
  pinMode(triacPin, OUTPUT);
  digitalWrite(triacPin, LOW);

  // Start zero cross circuit if not started yet
  if (!started) {
    pinMode(DIMMER_ZERO_CROSS_PIN, INPUT);
    attachInterrupt(DIMMER_ZERO_CROSS_INTERRUPT, callZeroCross, RISING);
    started = true;
  }

  if (operatingMode != DIMMER_COUNT && !timerStarted) {
    // Setup timer to fire every 50us @ 60Hz
    TCNT(DIMMER_TIMER) = 0;                      // Clear timer
    TCCRxA(DIMMER_TIMER) = TCCRxA_VALUE;         // Timer config byte A
    TCCRxB(DIMMER_TIMER) = TCCRxB_VALUE;         // Timer config byte B
    TIMSKx(DIMMER_TIMER) = 0x02;                 // Timer Compare Match Interrupt Enable
    OCRxA(DIMMER_TIMER) = 100 * 60 / acFreq - 1; // Compare value (frequency adjusted)
    timerStarted = true;
  }
}

void Dimmer::off() {
  lampState = false;
}

void Dimmer::on() {
  lampState = true;
}

void Dimmer::toggle() {
  lampState = !lampState;
}

bool Dimmer::getState() {
  return lampState;
}

uint8_t Dimmer::getValue() {
  return rampStartValue + ((int32_t) lampValue - rampStartValue) * rampCounter / rampCycles;
}

void Dimmer::set(uint8_t value) {
  if (value > 100) {
    value = 100;
  }

  if (value < minValue) {
    value = minValue;
  }

  if (operatingMode == DIMMER_RAMP) {
    rampStartValue = getValue();
    rampCounter = 0;
  } else if (operatingMode == DIMMER_COUNT) {
    pulses = 0;
    pulseCount = 0;
  }
  lampValue = value;
}

void Dimmer::set(uint8_t value, bool on) {
  set(value);
  lampState = on;
}

void Dimmer::setMinimum(uint8_t value) {
  if (value > 100) {
    value = 100;
  }

  minValue = value;

  if (lampValue < minValue) {
    set(value);
  }
}

void Dimmer::zeroCross() {
  if (operatingMode == DIMMER_COUNT) {
    // Check the MSB bit
    if (pulses >= DIMMER_MSB && pulseCount > 0) {
      pulseCount--;
    }

    pulses = pulses << 1;

    if (lampValue > pulseCount * DIMMER_SCALE) {
      *triacPinPort |= triacPinMask;
      pulses++;
      pulseCount++;
    } else {
      *triacPinPort &= ~triacPinMask;
    }
  } else {
    // Turn-off triac
    *triacPinPort &= ~triacPinMask;

    // Calculate triac time for the current cycle
    uint8_t value = getValue();
    if (value > 0 && lampState) {
      currentTriacTime = triacTime[value - 1];
    } else {
      currentTriacTime = 255;
    }

    // Increment the ramp counter until it reaches the total number of cycles for the ramp
    if (operatingMode == DIMMER_RAMP && rampCounter < rampCycles) {
      rampCounter++;
    }
  }
}

void Dimmer::triac() {
  if (operatingMode != DIMMER_COUNT && tmrCount >= currentTriacTime) {
    *triacPinPort |= triacPinMask;
  }
}
