/**
 * Arduino Fade example using an AC lamp and setting a minimum power level. This is useful for
 * lamps that cannot be dimmed to a very low level, like dimmable LED or CFL lamps.
 *
 * Copyright (c) 2016 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer dimmer(3, DIMMER_RAMP, 1.5);

void setup() {
  dimmer.begin();
  dimmer.setMinimum(20);
}

void loop() {
  dimmer.set(100);
  delay(2500);
  dimmer.set(0);
  delay(2500);
}
