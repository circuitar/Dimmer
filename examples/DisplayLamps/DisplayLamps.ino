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

#include "Dimmer.h"

//Set 3 dimerable lights at pins 3,5 and 6
Dimmer lamp1(3, RAMP_MODE, 100, 50, ON);
Dimmer lamp2(5, RAMP_MODE, 100, 50, OFF);
Dimmer lamp3(6, RAMP_MODE, 100, 50, OFF);

//Light powers animation
#define p1  20
#define p2  80
byte pot[6][3] = {
  {p2,  0, 0 },
  {p1, p1, 0 },
  {0 , p2, 0 },
  {0 , p1, p1},
  {0 ,  0, p2},
  {p1,  0, p1}
};

void setup() {
  //Start dimmers in ramp mode (smooth transitions) with speed 100.
  lamp1.begin();
  lamp2.begin();
  lamp3.begin();
}

void loop() {
  //Wave form animation
  for (byte i = 0; i < 3; i++) {
    lamp1.set(pot[i][0]);
    lamp2.set(pot[i][1]);
    lamp3.set(pot[i][2]);
    delay(400);
  }
}
