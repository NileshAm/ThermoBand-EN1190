#include <stdint.h>
#ifndef BLINKER_H
#define BLINKER_H
#include <Arduino.h>

class Blinker{
  public:
    Blinker(uint8_t pin);

    void setupBlinker();
    void blink(int interval);

  private:
    uint8_t _pin;
    bool _ledState;
    unsigned long _previousMillis;
};


#endif