/**
 * Use count mode to switch the load on and off only when the AC voltage crosses zero. In this
 * mode, the power is controlled by turning the triac on only for complete (half) cycles of the AC
 * sine wave. The power delivery is adjusted by counting the number of cycles that are activated.
 * This helps controlling higher, slower response loads (e.g. resistances) without introducing
 * any triac switching noise on the line.
 *
 * Copyright (c) 2016 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"

Dimmer dimmer(3, DIMMER_COUNT);

int value = 0;

void setup() {
  Serial.begin(9600);
  dimmer.begin();
}

void loop() {
  value += 5;
  if (value > 100) {
    value = 0;
  }

  dimmer.set(value);

  Serial.print("Power: ");
  Serial.print(value);
  Serial.println("%");
  
  delay(3000);
}
