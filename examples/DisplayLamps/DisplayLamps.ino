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

Dimmer dimmer;

#define p1  10
#define p2  40

byte pot[6][3] = {  {p2,  0, 0 }, 
                    {p1, p1, 0 },
                    {0 , p2, 0 }, 
                    {0 , p1, p1}, 
                    {0 ,  0, p2},
                    {p1,  0, p1}
                  };

void setup() {
  dimmer.attachZeroCross(2,0);
  dimmer.attachTriac(3);
  dimmer.attachTriac(5);
  dimmer.attachTriac(6);
  dimmer.init();
}

void loop(){
  wave1(200, 3);
  dimmer.initRamp();
  wave2(700, 3);
  OnOff(500, 3);
  dimmer.init();
  OnOff(300, 3);
  OnOff(100, 3);
}

void wave1(short delayms, byte times){
  for(byte k=0; k<times; k++){
    for(byte i=0; i<6; i++){
      for(byte j=0; j<3; j++){
        dimmer.set(j+1, pot[i][j]);
      }
      delay(delayms);
    }
  }
}

void wave2(short delayms, byte times){
  for(byte k=0; k<times; k++){
    for(byte i=0; i<6; i++){
      for(byte j=0; j<3; j++){
        dimmer.set(j+1, pot[i][j]==p2 ? 0 : p2);
      }
      delay(delayms);
    }
  }
}

void OnOff(short delayms, byte times){
  for(byte k=0; k<times; k++){
    for(byte i=0; i<6; i++){
      for(byte j=0; j<3; j++){
        dimmer.set(j+1, pot[i][j]==p2 ? p2 : 0);
      }
      delay(delayms);
    }
  }
}
