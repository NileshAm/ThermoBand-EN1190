// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into GPIO4 (D2 on ESP8266)
#define ONE_WIRE_BUS 4

// Create a OneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  Serial.println("Dallas Temperature Sensor Initialization");

  pinMode(A0, INPUT);

  // Start the Dallas Temperature library
  sensors.begin();
  sensors.setResolution(14);
}

void loop() {
  // Serial.print("Requesting temperature...");
  sensors.requestTemperatures(); // Request temperature readings
  // Serial.println("DONE");

  // Read temperature from first sensor
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    // Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("    ");
    Serial.println(analogRead(A0)*3.3*100/1023.0);
    // Serial.println(" °C");
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  delay(500); 
}