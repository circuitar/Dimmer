Dimmer Library for Arduino
==========================

This is the Arduino software library to control AC loads using triacs and a zero cross detector circuit. The library functions can be used to control the AC load power of each Triac independently, using a single zero cross circuit.

The reference circuit is based on the Triac and Zero Cross Nanoshields from Circuitar, and can be found at:

- Triac: https://www.circuitar.com.br/en/nanoshields/modules/triac/ 
- Zero Cross: https://www.circuitar.com.br/en/nanoshields/modules/zero-cross/

There are different ways to implement zero cross detector circuits. This library is based on the implementation above, but it can be easily adapted to use any type of zero cross detector circuit.

To install, copy the library under <arduinoSketchFolder>/libraries/

This library needs TimerOne library which can be found here: https://github.com/PaulStoffregen/TimerOne

---

The following examples are provided:

- **displayLamps**: This program transitions 3 lamps in playfull ways to show how they can be controlled. (can be extended to 10 lamps).
- **randomLamps**: Control 3 dimmable lamps with random values (can be extended to 10 lamps).
- **waveLamps**: Control 3 dimmable lamps in a wave form (can be extended to 10 lamps).

---

Usage:

Use the library functions `attachZeroCross()` and `attachTriac()` to assign each triac and the zero cross shield to the program.

To start controlling the lamps, initialize with `init()` or `initRamp()` to make smooth transitions between power levels. In both cases the lamps will start with 50% of the AC power.

The triacs can be controlled using the functions: `set(lamp, power)`, `on()`, `on(lamp)`, `off()`, `off(lamp)` and `toogle(lamp)`.

---

Copyright (c) Circuitar Eletrônicos - 2015

This software is released under an MIT license. See the attached LICENSE file for details.

