#include "Blinker.h"

Blinker::Blinker(uint8_t pin)
  : _pin(pin),
    _previousMillis(0),
    _ledState(LOW)
{
}

void Blinker::setupBlinker(){
  pinMode(_pin, OUTPUT);
}

void Blinker::blink(int interval){
  unsigned long currentMillis = millis();
  if (currentMillis - _previousMillis >= interval) {
    _previousMillis = currentMillis;
    _ledState = !_ledState;
    digitalWrite(_pin, _ledState);
  }
}