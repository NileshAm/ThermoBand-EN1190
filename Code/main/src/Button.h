#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
  public:
    // Constructor: specify the digital pin, long press delay (ms), and optional debounce delay (ms).
    Button(uint8_t pin, unsigned long longPressDelay, unsigned long debounceDelay = 50);

    // Initialize the button; call this in setup().
    void begin();

    // Non-blocking update method; call this in every loop() iteration.
    void update();

    // Returns true if a short press event occurred (button released and held less than long press delay).
    // Clears the event flag once called.
    bool wasShortPress();

    // Returns true if a long press event occurred (button released after long press delay).
    // Clears the event flag once called.
    bool wasLongPress();

    // Returns true if the button is currently pressed and held for less than long press delay.
    bool isShortPressed();

    // Returns true if the button is currently pressed and held for at least the long press delay.
    bool isLongPressed();

  private:
    uint8_t _pin;
    unsigned long _longPressDelay;
    unsigned long _debounceDelay;
    bool _lastReading;             // Last stable reading.
    bool _currentReading;          // Current stable reading.
    unsigned long _lastDebounceTime; // Last time the reading changed.
    unsigned long _pressTime;      // Time when button was pressed.
    unsigned long _releaseTime;    // Time when button was released.
    bool _shortPressEvent;         // Flag for a short press event.
    bool _longPressEvent;          // Flag for a long press event.
};

#endif // BUTTON_H
