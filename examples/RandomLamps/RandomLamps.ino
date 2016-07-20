/**
 * Activate 3 dimmable lamps randomly.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer lamp1(3);
Dimmer lamp2(5);
Dimmer lamp3(6);

void setup() {
  lamp1.begin();
  lamp2.begin();
  lamp3.begin();

  randomSeed(analogRead(0));
}

void loop() {
  lamp1.set(random(0, 100));
  delay(500);

  lamp2.set(random(0, 100));
  delay(500);
  
  lamp3.set(random(0, 100));
  delay(500);
}
