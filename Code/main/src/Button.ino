#include "Button.h"

Button myButton(5, 2000);

void setup() {
  myButton.begin();
}

void loop() {
  myButton.update();

  // Check for event-based press detection.
  if (myButton.wasShortPress()) {
    logMessage("Short press event detected");
  }
  if (myButton.wasLongPress()) {
    logMessage("Long press event detected");
  }
  
  // For continuous state detection, check if the button is currently pressed.
  if (myButton.isShortPressed()) {
    logMessage("Button is currently pressed (short)");
  }
  if (myButton.isLongPressed()) {
    logMessage("Button is currently pressed (long)");
  }

}
