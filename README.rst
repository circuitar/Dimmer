Dimmer Library for Arduino
==============

This is an Arduino software library to control AC loads using triacs and a zero cross detector circuit. The library methods can be used to control the AC load power for multiple triacs independently, using a single shared zero-cross circuit.

* Source code: https://github.com/circuitar/Dimmer
* Documentation: http://dimmer.readthedocs.org/
* Reference Board: `Triac Nanoshield`_  and `Zero Cross Nanoshield`_ from Circuitar_

There are different ways to implement zero cross detector circuits. This library is based on the implementation above, but it can be easily adapted to use any type of zero cross detector circuit.

To install, just click **Download ZIP** and install it using **Sketch > Include Library... > Add .ZIP Library** in the Arduino IDE.

The following examples_ are provided:

* Fade_ Arduino Fade example using an AC lamp.
* FadeMinimum_ Arduino Fade example using an AC lamp and setting a minimum power level (useful for dimmable LED or CFL lamps).
* RandomLamps_ Control 3 dimmable lamps with random values (can be extended to 10 lamps).
* WaveLamps_ Control 3 dimmable lamps in a wave form (can be extended to 10 lamps).
* CountMode_ Control high, low response AC loads without introducing noise using count mode.

.. _`Triac Nanoshield`: https://www.circuitar.com/nanoshields/modules/triac/
.. _`Zero Cross Nanoshield`: https://www.circuitar.com/nanoshields/modules/zero-cross/
.. _Circuitar: https://www.circuitar.com/
.. _examples: https://github.com/circuitar/Dimmer/tree/master/examples/
.. _Fade: https://github.com/circuitar/Dimmer/blob/master/examples/Fade/Fade.ino
.. _FadeMinimum: https://github.com/circuitar/Dimmer/blob/master/examples/FadeMinimum/FadeMinimum.ino
.. _RandomLamps: https://github.com/circuitar/Dimmer/blob/master/examples/RandomLamps/RandomLamps.ino
.. _WaveLamps: https://github.com/circuitar/Dimmer/blob/master/examples/WaveLamps/WaveLamps.ino
.. _CountMode: https://github.com/circuitar/Dimmer/blob/master/examples/CountMode/CountMode.ino

----

Copyright (c) 2015 Circuitar  
All rights reserved.

This software is released under an MIT license. See the attached LICENSE file for details.
