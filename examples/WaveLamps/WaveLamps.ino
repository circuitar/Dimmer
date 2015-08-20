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

#include "Dimmer.h"

//store dimm lights in array
Dimmer dimm[] = { Dimmer(3, RAMP_MODE, 100, 50, ON), 
                  Dimmer(5, RAMP_MODE, 100, 50, OFF),
                  Dimmer(6, RAMP_MODE, 100, 50, OFF) 
                };
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
    dimm[0].begin();
    dimm[1].begin();
    dimm[2].begin();
}

void loop(){
  for(byte i=0; i<6; i++){
    for(byte j=0; j<3; j++){
      dimm[j].set(pot[i][j]);
    }
    delay(200);
  }
}