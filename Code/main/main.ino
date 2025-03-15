// ######################### Do not change this Code ###########################################
#include "src/WIFI_Programmer/WIFI_Programmer.h"
#include "src/WIFI_Programmer/SerialWebLogger.h"
// #############################################################################################
#include "src/DigitalTempSensor/DigitalTempSensor.h"
#include "src/Blinker/Blinker.h"

void setup(){
  // ######################### Do not change this Code ###########################################
  Serial.begin(115200);
  wifiSetupOTA();
  initSerialWebLogger();
  // #############################################################################################
  setupSensor(4);
  tempSensorBegin();
  setResolution(12);
  
  setupBLinker();
}

void loop(){
  // ######################### Do not change this Code ###########################################
  otaHandle();
  updateSerialWebLogger();
  // #############################################################################################

  // logMessage("start");
  logMessage(readTempSensor());

  blink(200);
}