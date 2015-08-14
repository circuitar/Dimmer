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

DimmerControl lamp1(3, 50, ON);
DimmerControl lamp2(5, 50, OFF);
DimmerControl lamp3(6, 50, OFF);

int value;

void setup() {
  lamp1.begin(NORMAL_MODE, 100);
  lamp2.begin(RAMP_MODE, 100);
  lamp3.begin(RAMP_MODE, 100);
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

