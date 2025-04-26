#include "DigitalTempSensor.h"

void setup() {
  Serial.begin(115200);
  setupSensor(4);   // Initialize sensor on GPIO4 (or your desired pin)
  tempSensorBegin();
  setResolution(12);
}

void loop() {
  float temp = readTempSensor();
  Serial.println("Temperature: " + String(temp) + " °C");
  delay(3000);
}
