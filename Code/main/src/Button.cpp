#include "Button.h"

Button::Button(uint8_t pin, unsigned long longPressDelay, unsigned long debounceDelay)
  : _pin(pin),
    _longPressDelay(longPressDelay),
    _debounceDelay(debounceDelay),
    _lastReading(HIGH),
    _currentReading(HIGH),
    _lastDebounceTime(0),
    _pressTime(0),
    _releaseTime(0),
    _shortPressEvent(false),
    _longPressEvent(false)
{
}

void Button::begin() {
  pinMode(_pin, INPUT_PULLUP); // Assuming button is active LOW.
  _lastReading = digitalRead(_pin);
  _currentReading = _lastReading;
  _lastDebounceTime = millis();
}

void Button::update() {
  bool reading = digitalRead(_pin);

  // If reading changed, reset the debounce timer.
  if (reading != _lastReading) {
    _lastDebounceTime = millis();
  }
  
  // If the reading has been stable beyond the debounce delay, update the state.
  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    if (reading != _currentReading) {
      _currentReading = reading;
      
      if (_currentReading == LOW) {  // Button pressed
        _pressTime = millis();
      } else {  // Button released
        _releaseTime = millis();
        unsigned long duration = _releaseTime - _pressTime;
        if (duration >= _longPressDelay) {
          _longPressEvent = true;
        } else if (duration >= _debounceDelay) {
          _shortPressEvent = true;
        }
      }
    }
  }
  
  _lastReading = reading;
}

bool Button::wasShortPress() {
  if (_shortPressEvent) {
    _shortPressEvent = false;
    return true;
  }
  return false;
}

bool Button::wasLongPress() {
  if (_longPressEvent) {
    _longPressEvent = false;
    return true;
  }
  return false;
}

bool Button::isShortPressed() {
  // While button is pressed, check if duration is less than long press delay.
  if (_currentReading == LOW) {
    unsigned long duration = millis() - _pressTime;
    return (duration < _longPressDelay);
  }
  return false;
}

bool Button::isLongPressed() {
  // While button is pressed, check if duration has exceeded the long press delay.
  if (_currentReading == LOW) {
    unsigned long duration = millis() - _pressTime;
    return (duration >= _longPressDelay);
  }
  return false;
}
