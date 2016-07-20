/**
 * Arduino Fade example using an AC lamp.
 *
 * Copyright (c) 2016 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer dimmer(3, DIMMER_RAMP, 1.5);

void setup() {
  dimmer.begin();
}

void loop() {
  dimmer.set(100);
  delay(1500);
  dimmer.set(0);
  delay(1500);
}
