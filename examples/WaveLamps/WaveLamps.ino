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
DimmerControl dimm[] = { DimmerControl(3, 50, ON), 
                         DimmerControl(5, 50, OFF),
                         DimmerControl(6, 50, OFF) 
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
    dimm[0].begin(RAMP_MODE, 100);
    dimm[1].begin(RAMP_MODE, 100);
    dimm[2].begin(RAMP_MODE, 100);
}

void loop(){
  for(byte i=0; i<6; i++){
    for(byte j=0; j<3; j++){
      dimm[j].set(pot[i][j]);
    }
    delay(200);
  }
}