#include "Blinker.h"

Blinker bl(2);

void setup() {
  bl.setupBlinker();

}

void loop() {
  bl.blink(1200);

}
