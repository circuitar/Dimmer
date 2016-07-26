/**
 * Dim many lamps simultaneously.
 *
 * Copyright (c) 2016 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer dimmers[] = {
  Dimmer(3, DIMMER_RAMP),
  Dimmer(5, DIMMER_RAMP),
  Dimmer(6, DIMMER_RAMP),
  Dimmer(9, DIMMER_RAMP),
  Dimmer(4, DIMMER_RAMP),
  Dimmer(7, DIMMER_RAMP),
  Dimmer(A2, DIMMER_RAMP),
  Dimmer(A3, DIMMER_RAMP),
  Dimmer(A4, DIMMER_RAMP),
  Dimmer(A5, DIMMER_RAMP)
};

void setup() {
  for(int i = 0; i < sizeof(dimmers) / sizeof(Dimmer); i++) {
    dimmers[i].begin();
  }
}

void loop() {
  for(int i = 0; i < sizeof(dimmers) / sizeof(Dimmer); i++) {
    dimmers[i].set(100);
  }
  delay(1500);

  for(int i = 0; i < sizeof(dimmers) / sizeof(Dimmer); i++) {
    dimmers[i].set(0);
  }
  delay(1500);
}
