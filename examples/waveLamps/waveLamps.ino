/**
 * Control 3 dimmable lamps in a wave form
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

#define p1  20
#define p2  60

byte pot[6][3] = {  {p2,  0, 0 }, 
                    {p1, p1, 0 },
                    {0 , p2, 0 }, 
                    {0 , p1, p1}, 
                    {0 ,  0, p2},
                    {p1,  0, p1}
                  };

void setup() {
  dimmer1.attachZeroCross(2,0);
  dimmer1.attachTriac(3);
  dimmer1.attachTriac(5);
  dimmer1.attachTriac(6);
  dimmer1.init();
}

void loop(){
  for(byte i=0; i<6; i++){
    for(byte j=0; j<3; j++){
      dimmer1.set(j+1, pot[i][j]);
    }
    delay(200);
  }
}
