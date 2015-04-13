Dimmer_Library
=======================

This is the Arduino software library to access the Circuitar Triac_Nanoshield alongside with Circuitar Zerocross_Nanoshield.

The library is used to control the AC load power of each Triac.

The reference circuit from both shields can be found at: 
    https://www.circuitar.com.br/en/nanoshields/modules/triac/ 
    https://www.circuitar.com.br/en/nanoshields/modules/zero-cross/

This library can be addapted to use any zero cross dector shield and triac shield.

To install, copy the lybrary under <arduinoSketchFolder>/libraries/

This library needs TimerOne library which can be found here: https://github.com/PaulStoffregen/TimerOne

---

The following example is provided:

- **Lamps**: Control 3 dimmerable lamps with random values (Extended to 10 lamps).


---

Usage:

Use the library functions `attachZeroCross()` and `attachTriac()` to assign each triac and the zero cross shield to the program.

To start controlling the lamps initialize with `ini()` or `initRamp()` to make smooths transitions between power setups. In both cases the set lamps will start with 50% of the AC power.

The triacs can be controlled using the functions: `set(lamp, power)`, `on()`, `on(lamp)`, `off()`, `off(lamp)` and `toogle(lamp)`.


---

Copyright (c) Circuitar Eletrônicos - 2015

This software is released under an MIT license. See the attached LICENSE file for details.