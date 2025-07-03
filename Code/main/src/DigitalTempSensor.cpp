#include "DigitalTempSensor.h"
#include <Arduino.h>

// Global pointer for OneWire instance and sensor object.
// These are declared as static to limit their scope to this file.
static OneWire* oneWirePtr = nullptr;
static DallasTemperature sensors((OneWire*)nullptr);

void setupSensor(int pin) {
  // Delete any previous OneWire instance (if present)
  if (oneWirePtr != nullptr) {
    delete oneWirePtr;
  }
  // Create a new OneWire instance on the specified pin.
  oneWirePtr = new OneWire(pin);
  
  // Initialize the DallasTemperature object with the new OneWire instance.
  sensors = DallasTemperature(oneWirePtr);
  
  Serial.println("Dallas Temperature Sensor Initialization");
}

void setResolution(int res) {
  sensors.setResolution(res);
}

void tempSensorBegin() {
  sensors.begin();
}

float readTempSensor() {
  sensors.requestTemperatures(); // Send the command to get temperature readings.
  
  // Read temperature from the first sensor on the bus.
  float tempC = sensors.getTempCByIndex(0);
  
  // Check if the sensor reading is valid.
  if (tempC != DEVICE_DISCONNECTED_C) {
    return tempC;
  } else {
    return 0; // Return 0 if the sensor is disconnected or an error occurred.
  }
}
