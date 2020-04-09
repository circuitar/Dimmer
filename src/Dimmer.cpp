/** @file
 * This is a library to control the intensity of dimmable AC lamps or other AC loads using triacs.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Arduino.h"
#include "Dimmer.h"
////#include <util/atomic.h>

// Timer configuration macros_TIMER(X)
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
#define _Timer(X) Timer ## X
#define Timer(X) _Timer(X)
#define _TIM(X) TIM ## X
#define TIM(X) _TIM(X)
#define _NVIC_TIMER(X) NVIC_TIMER ## X
#define NVIC_TIMER(X) _NVIC_TIMER(X)

// Helper macros
#define DIMMER_PULSES_TO_VALUE(pulses, bits) ((uint16_t)(pulses) * 100 / (bits))

#if DIMMER_TIMER == 2

// 8-bit timer
#define TCCRxA_VALUE 0x02 // CTC mode
#define TCCRxB_VALUE 0x02 // Prescaler = 8 => Cycle = 0.5us @ 16MHz

#elif (DIMMER_TIMER == 1 || DIMMER_TIMER == 3 || DIMMER_TIMER == 4 || DIMMER_TIMER == 5)

// 16-bit timer
#define TCCRxA_VALUE 0x00 // CTC mode
#define TCCRxB_VALUE 0x0A // CTC mode, Prescaler = 8 => Cycle = 0.5us @ 16MHz

#endif

// Time table to set triacs (50us tick)
//   This is the number of ticks after zero crossing that we should wait before activating the
//   triac, where the array index is the applied power from 1 to 100, minus one. This array is
//   calculated based on the equation of the effective power applied to a resistive load over
//   time, assuming a 60HZ sinusoidal wave.
static const uint8_t powerToTicks[] PROGMEM = {
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
static Dimmer* dimmmers[DIMMER_MAX_TRIAC];     // Pointers to all registered dimmer objects
static uint8_t dimmerCount = 0;                // Number of registered dimmer objects

// Triac pin and timing variables. Using global arrays to make ISR fast.
#if defined(ARDUINO_ARCH_AVR) 
  static volatile uint8_t* triacPinPorts[DIMMER_MAX_TRIAC]; // Triac ports for registered dimmers
  static uint8_t triacPinMasks[DIMMER_MAX_TRIAC];           // Triac pin mask for registered dimmers
#elif defined(ARDUINO_ARCH_STM32F1) || defined(ARDUINO_ARCH_STM32F4) || defined(ARDUINO_ARCH_STM32) 
  static volatile uint32_t* triacPinPorts[DIMMER_MAX_TRIAC]; // Triac ports for registered dimmers
  static uint32_t triacPinMasks[DIMMER_MAX_TRIAC];           // Triac pin mask for registered dimmers
#endif

#if defined(ARDUINO_ARCH_STM32) 
  HardwareTimer MyTim = HardwareTimer(TIM(DIMMER_TIMER));

#endif  

static uint8_t triacTimes[DIMMER_MAX_TRIAC];              // Triac time for registered dimmers

// Timer ticks since zero crossing
static uint8_t tmrCount = 0;

// Global state variables
bool Dimmer::started = false; // At least one dimmer has started

//================== added by Hamid ==================

bool Auto_Main_frequency_detection=false;
volatile uint8_t sample_for_Main_frequency_detection;
unsigned long lastTime_for_Main_frequency_detection;
volatile unsigned long frequency_detection_timings_sum;

//====================================================
	
// Zero cross interrupt
void callZeroCross() {
	
  //================== added by Hamid ===================
  if(Auto_Main_frequency_detection){

	if(sample_for_Main_frequency_detection <= NUMBER_OF_SAMLPLES_FOR_MAIN_FREQUENCY_DETECTION){
		
		const long time = micros();
		const unsigned long duration = time - lastTime_for_Main_frequency_detection;
	
		if (duration <= (1000.0/MIN_INPUT_FREQUENCY)*1000.0/2.0 && duration >= (1000.0/MAX_INPUT_FREQUENCY)*1000.0/2.0){ 
			if(sample_for_Main_frequency_detection!=0) frequency_detection_timings_sum += duration; // dump first one
			sample_for_Main_frequency_detection++;
		}
		lastTime_for_Main_frequency_detection = time;
	}
	return;
  }
  //====================================================
	

  // Clear the timer and enable nested interrupts so that the timer can be updated while this ISR is executed
  #if defined(ARDUINO_ARCH_AVR) 
   TCNT(DIMMER_TIMER) = 0;
   //sei();
  #elif defined(ARDUINO_ARCH__STM32F1) || defined(ARDUINO_ARCH__STM32F4) 
   //Timer(DIMMER_TIMER).setCount(0); //The new count value to set. If this value exceeds the timer’s overflow value, it is truncated to the overflow value.
   Timer(DIMMER_TIMER).refresh(); //This will reset the counter to 0 in upcounting mode (the default). It will also update the timer’s prescaler and overflow, if you have set them up to be changed using HardwareTimer::setPrescaleFactor() or HardwareTimer::setOverflow().
  #elif defined(ARDUINO_ARCH_STM32)
   MyTim.setCount(0);
  #endif
  
  interrupts();
  tmrCount = 0;

  // Turn off all triacs and disable further triac activation before anything else
  for (uint8_t i = 0; i < dimmerCount; i++) {
    *triacPinPorts[i] &= ~triacPinMasks[i];
    triacTimes[i] = 255;
  }

  // Process each registered dimmer object
  for (uint8_t i = 0; i < dimmerCount; i++) {
    dimmmers[i]->zeroCross();

    // If triac time was already reached, activate it
    if (tmrCount >= triacTimes[i]) {
      *triacPinPorts[i] |= triacPinMasks[i];
    }
  }
}

// Timer interrupt
#if defined(ARDUINO_ARCH_STM32F1) || defined(ARDUINO_ARCH_STM32F4) 
  void onTimerISR()
  
#elif defined(ARDUINO_ARCH_STM32)
  void onTimerISR(HardwareTimer*)

#elif defined(ARDUINO_ARCH_AVR)
  ISR(TIMER_COMPA_VECTOR(DIMMER_TIMER))
  
#endif	  
  {	
    // Increment ticks
    if (tmrCount < 254) {
      tmrCount++;
    }

    // Process each registered triac and turn it on if needed
    for (uint8_t i = 0; i < dimmerCount; i++) {
      if (tmrCount == triacTimes[i]) {
        *triacPinPorts[i] |= triacPinMasks[i];
      }
    }
  }

// Constructor
Dimmer::Dimmer(uint8_t out_dimmer_pin, uint8_t zc_dimmer_pin, uint8_t mode, double rampTime, uint8_t freq) :
        triacPin(out_dimmer_pin),
		zc_pin(zc_dimmer_pin),
		operatingMode(mode),
        lampState(false),
        lampValue(0),
        rampStartValue(0),
        rampCounter(1),
        rampCycles(1),
        pulsesHigh(0),
        pulsesLow(0),
        pulseCount(0),
        pulsesUsed(0),
		_rampTime(rampTime),
        acFreq(freq)	{
			
  if (dimmerCount < DIMMER_MAX_TRIAC) {
    // Register dimmer object being created
    dimmerIndex = dimmerCount;
    dimmmers[dimmerCount++] = this;
    triacPinPorts[dimmerIndex] = portOutputRegister(digitalPinToPort(out_dimmer_pin));
    triacPinMasks[dimmerIndex] = digitalPinToBitMask(out_dimmer_pin);
  }

  if (mode == DIMMER_RAMP && freq!=false) {
    setRampTime(rampTime);
  }
}

void Dimmer::begin(uint8_t value, bool on ) {
  // Initialize lamp state and value
  set(value, on);

  // Initialize triac pin
  pinMode(triacPin, OUTPUT);
  digitalWrite(triacPin, LOW);
  
  if (!started) {
	  
	if (acFreq == false) Auto_Main_frequency_detection = true;
	
    // Start zero cross circuit if not started yet
    pinMode(zc_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(zc_pin), callZeroCross, RISING);

  //================== added by Hamid =================
  if(Auto_Main_frequency_detection){
	
	while(sample_for_Main_frequency_detection <= NUMBER_OF_SAMLPLES_FOR_MAIN_FREQUENCY_DETECTION){
		// Zerocross-Interrupt doing his job
	}

	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //Creates a block of code that is guaranteed to be executed atomically. Upon entering the block the Global Interrupt Status flag in SREG is disabled, and re-enabled upon exiting the block from any exit path.
	noInterrupts();	// go atomic level - avoid interrupts to surely get the previously received data
	
	/* If the volatile variable is bigger than a byte (e.g. a 16 bit int or a 32 bit long), then the microcontroller can not read it in one step, because it is an 8 bit microcontroller. This means that while your main code section (e.g. your loop) reads the first 8 bits of the variable, the interrupt might already change the second 8 bits. This will produce random values for the variable.
		Remedy:
		While the variable is read, interrupts need to be disabled, so they can’t mess with the bits, while they are read. There are several ways to do this:
	*/
	float Main_frequency;
	Main_frequency = (1/((frequency_detection_timings_sum/NUMBER_OF_SAMLPLES_FOR_MAIN_FREQUENCY_DETECTION)/1000000.0))/2.0 ;// ( /2) because zerocross occur in half cycle
	acFreq = round(Main_frequency);
	Auto_Main_frequency_detection= false;// must be after all to execute "return" in ISR
    
	if (operatingMode == DIMMER_RAMP ) {
		setRampTime(_rampTime);
	}
	
	interrupts();							// let interrupt do its job
	//}
  }
  
  //======================================================
    // Setup timer to fire every 50us @ 60Hz
	
  #if defined(ARDUINO_ARCH_STM32F1) || defined(ARDUINO_ARCH__STM32F4) 
  
    Timer(DIMMER_TIMER).setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
    Timer(DIMMER_TIMER).setPeriod(50*(60.0/acFreq)); // in microseconds
    Timer(DIMMER_TIMER).setCompare(TIMER_CH1, 1);      // Interrupt 1 count after each update, overflow might be small
    Timer(DIMMER_TIMER).attachInterrupt(TIMER_CH1, onTimerISR);
	//nvic_irq_set_priority(NVIC_TIMER(DIMMER_TIMER), 15);// interrupt must not be preempted. highest interrupt priority is 0. All other interrupts have been initialized to priority level 16. See nvic_init().
	
  #elif defined(ARDUINO_ARCH_STM32)
	
	MyTim.setMode(2, TIMER_OUTPUT_COMPARE);
	MyTim.setOverflow(50*(60.0/acFreq), MICROSEC_FORMAT);
	MyTim.attachInterrupt(onTimerISR);
	MyTim.resume();

  #elif defined(ARDUINO_ARCH_AVR)
  
	TCNT(DIMMER_TIMER) = 0;                      // Clear timer
    TCCRxA(DIMMER_TIMER) = TCCRxA_VALUE;         // Timer config byte A
    TCCRxB(DIMMER_TIMER) = TCCRxB_VALUE;         // Timer config byte B
    TIMSKx(DIMMER_TIMER) = 0x02;                 // Timer Compare Match Interrupt Enable
    OCRxA(DIMMER_TIMER) = 100 * 60.0 / acFreq - 1; // Compare value (frequency adjusted)  
	
  #endif
    started = true;
  }
}

void Dimmer::off() {
  lampState = false;
}

void Dimmer::on() {
  lampState = true;
}

void Dimmer::toggle() {
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	noInterrupts();	
    lampState = !lampState;
	interrupts();
  //}
}

bool Dimmer::getState() {
  return lampState;
}

uint8_t Dimmer::getValue() {
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	noInterrupts();
	uint8_t val = rampStartValue + ((int32_t) lampValue - rampStartValue) * rampCounter / rampCycles ;
	interrupts();
    return val;
  //}
}

void Dimmer::set(uint8_t value) {
  if (value > 100) {
    value = 100;
  }

  if (value < minValue) {
    value = minValue;
  }

  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	noInterrupts();
    if (value != lampValue) {
      if (operatingMode == DIMMER_RAMP) {
        rampStartValue = getValue();
        rampCounter = 0;
      } else if (operatingMode == DIMMER_COUNT) {
        pulsesHigh = 0;
        pulsesLow = 0;
        pulseCount = 0;
        pulsesUsed = 0;
      }
      lampValue = value;
    }
    interrupts();
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

  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	noInterrupts();
    if (lampValue < minValue) {
      set(value);
    }
	interrupts();
  //}
}

void Dimmer::setRampTime(double rampTime) {
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	noInterrupts();
    rampTime = rampTime * 2 * acFreq;
    rampCycles = rampTime > 0xFFFF ? 0xFFFF : rampTime;
    rampCounter = rampCycles;
	interrupts();
  //}
}

void Dimmer::zeroCross() {
  if (operatingMode == DIMMER_COUNT) {
    // Remove MSB from buffer and decrement pulse count accordingly
    if (pulseCount > 0 && (pulsesHigh & (1ULL << 35))) {
      pulsesHigh &= ~(1ULL << 35);
      if (pulseCount > 0) {
        pulseCount--;
      }
    }

    // Shift 100-bit buffer to the right
    pulsesHigh <<= 1;
    if (pulsesLow & (1ULL << 63)) {
      pulsesHigh++;
    }
    pulsesLow <<= 1;

    // Turn next half cycle on if number of pulses is low within the used buffer
    if (lampValue > DIMMER_PULSES_TO_VALUE(pulseCount, pulsesUsed)) {
      // Turn dimmer on at zero crossing time, @10 ticks (500us)
      triacTimes[dimmerIndex] = 10;
      pulsesLow++;
      pulseCount++;
    }

    // Update number of bits used in the buffer
    if (pulsesUsed < 100) {
      pulsesUsed++;
    }

  } else {
    // Calculate triac time for the current cycle
    uint8_t value = getValue();
    if (value > 0 && lampState) {
      triacTimes[dimmerIndex] = pgm_read_byte(&powerToTicks[value - 1]);
    }

    // Increment the ramp counter until it reaches the total number of cycles for the ramp
    if (operatingMode == DIMMER_RAMP && rampCounter < rampCycles) {
      rampCounter++;
    }
  }
}

uint8_t Dimmer::getAcFrequency(){
	return acFreq;
}
