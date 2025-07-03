#ifndef DIGITALTEMPSENSOR_H
#define DIGITALTEMPSENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

// Initializes the sensor on the given pin.
void setupSensor(int pin);

// Sets the resolution for temperature readings.
void setResolution(int res);

// Starts the sensor (must be called after setupSensor).
void tempSensorBegin();

// Requests and returns the temperature in °C from the first sensor.
float readTempSensor();

#endif // DIGITALTEMPSENSOR_H
