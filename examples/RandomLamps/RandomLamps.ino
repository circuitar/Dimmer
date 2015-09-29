/**
 * Control 3 dimmable lamps
 *
 * Copyright (c) 2015 Circuitar
 * All rights reserved.
 *
 * This software is released under a BSD license. See the attached LICENSE file for details.
 *
 * http://www.circuitar.com.br
 *
 */

#include "Dimmer.h"

Dimmer lamp1(3, NORMAL_MODE, 50, ON, 100);
Dimmer lamp2(5, RAMP_MODE, 50, OFF, 100);
Dimmer lamp3(6, RAMP_MODE, 50, OFF, 100);

int value;

void setup() {
  lamp1.begin();
  lamp2.begin();
  lamp3.begin();
  randomSeed(analogRead(0));
}

void loop(){
  value=random(0,256);
  lamp1.set(value);
  delay(500);
  value=random(0,256);
  lamp2.set(value);
  delay(500);
  value=random(0,256);
  lamp2.set(value);
  delay(500);
};

