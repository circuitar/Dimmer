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

#include <TimerOne.h>
#include "Dimmer.h"

Dimmer dimmer1;

void setup() {
  Serial.begin(9600);
  dimmer1.attachZeroCross(2,0);
  dimmer1.attachTriac(3);
  dimmer1.attachTriac(5);
  dimmer1.attachTriac(6);
  dimmer1.init();
  randomSeed(analogRead(0));
}

void loop(){
  char lamp=random(1,4);
  char value=random(0,256);
  dimmer1.set(lamp, value);
  delay(1000);
};

