/**
 * Control 3 dimmable lamps. This program transitions 3 in playful ways.
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

DimmerControl lamp1(3, 50, ON);
DimmerControl lamp2(5, 50, OFF);
DimmerControl lamp3(5, 50, OFF);

//Light powers animation
#define p1  10
#define p2  40
byte pot[6][3] = {
  {p2,  0, 0 },
  {p1, p1, 0 },
  {0 , p2, 0 },
  {0 , p1, p1},
  {0 ,  0, p2},
  {p1,  0, p1}
};

void setup() {
  lamp1.begin(NORMAL_MODE);
  lamp2.begin(RAMP_MODE);
  lamp2.begin(NORMAL_MODE);
}

void loop() {
  //Wave form animation
  for (byte i = 0; i < 6; i++) {
    lamp1.set(pot[i][0]);
    lamp2.set(pot[i][1]);
    lamp3.set(pot[i][2]);
    delay(400);
  }
}
