#include "Blinker.h"

int ledState = LOW;

unsigned long previousMillis = 0;

void setupBLinker(){
  pinMode(LED_BUILTIN, OUTPUT);
}

void blink(int interval){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;  // Note that this switches the LED *off*
    } else {
      ledState = LOW;  // Note that this switches the LED *on*
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}