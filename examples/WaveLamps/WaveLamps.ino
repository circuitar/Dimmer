/**
 * Control 3 dimmable lamps in a wave form.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer dimmers[] = {
  Dimmer(3, DIMMER_RAMP),
  Dimmer(5, DIMMER_RAMP),
  Dimmer(6, DIMMER_RAMP)
};

#define P1 20         // Power level 1 = 20%
#define P2 60         // Power level 2 = 60%

byte power[6][3] = {
  { P2,  0,  0 },
  { P1, P1,  0 },
  {  0, P2,  0 },
  {  0, P1, P1 },
  {  0,  0, P2 },
  { P1,  0, P1 }
};

void setup() {
  dimmers[0].begin();
  dimmers[1].begin();
  dimmers[2].begin();
}

void loop() {
  for(int i = 0; i < 6; i++) {
    for(int j = 0; j < 3; j++) {
      dimmers[j].set(power[i][j]);
    }

    delay(1000);
  }
}
