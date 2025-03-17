// ######################### Do not change this Code ###########################################
#include "src/WIFI_Programmer/WIFI_Programmer.h"
#include "src/WIFI_Programmer/SerialWebLogger.h"
// #############################################################################################
#include "src/DigitalTempSensor/DigitalTempSensor.h"
#include "src/Blinker/Blinker.h"
#include "src/Button/Button.h"

// ## Dont use D4/GPIO2, as its reserved for LED_BUILTIN 

Button btn1(5, 2000);
Button btn2(0, 2000);
// Button btn3(10, 2000);

int redPin = 14; 
int greenPin = 12;
int bluePin = 13;

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

  btn1.begin();
  btn2.begin();
  // btn3.begin();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  // pinMode(15, OUTPUT);
  // digitalWrite(15, HIGH);
}
bool RED = 1;
bool GREEN = 1;
bool BLUE = 1;

void loop(){
  // ######################### Do not change this Code ###########################################
  otaHandle();
  updateSerialWebLogger();
  // #############################################################################################

  
  btn1.update();
  btn2.update();
  // btn3.update();


  if (btn1.wasShortPress()) {
    RED = !RED;
    digitalWrite(redPin, RED);
    digitalWrite(greenPin, GREEN);
    digitalWrite(bluePin, BLUE);
    logMessage("btn1");
  }
  if (btn2.wasShortPress()) {
    GREEN = !GREEN;
    digitalWrite(redPin, RED);
    digitalWrite(greenPin, GREEN);
    digitalWrite(bluePin, BLUE);
    logMessage("btn2");
  }

  logMessage(readTempSensor());

  blink(200);
}