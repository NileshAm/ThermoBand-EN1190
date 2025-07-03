#include "WIFI_Programmer.h"      // Your WiFi/OTA module
#include "SerialWebLogger.h"

void setup() {
  Serial.begin(115200);
  // Minimal main: call wifiSetupOTA() to initialize WiFi (and OTA).
  // You can call with new credentials to update them; otherwise, omit parameters.
  wifiSetupOTA();
  initSerialWebLogger();

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
}

int ledState = LOW;

unsigned long previousMillis = 0;
const long interval = 1000;


void loop() {
  // Minimal main: call otaHandle() to process OTA requests.
  otaHandle();
  updateSerialWebLogger();
  // Place your main application code here.
  unsigned long currentMillis = millis();
  // logMessage(currentMillis);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;  // Note that this switches the LED *off*
      logMessage("LED OFF");
    } else {
      ledState = LOW;  // Note that this switches the LED *on*
      logMessage("LED ON");
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}
