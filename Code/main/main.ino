// ######################### Do not change this Code ###########################################
#include "src/WIFI_Programmer.h"
#include "src/SerialWebLogger.h"
// #############################################################################################
#include "src/DigitalTempSensor.h"
#include "src/Blinker.h"
#include "src/Button.h"
#include "src/HTTPRequest.h"

#include <EEPROM.h>

// EEPROM configuration
#define EEPROM_SIZE 160
#define EEPROM_SSID_ADDR 0
#define EEPROM_PASS_ADDR 32
#define EEPROM_UID_ADDR 96
#define EEPROM_MAC_ADDR 128
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define MAX_UID_LEN 32
#define MAX_MAC_LEN 18
// Hardcoded default credentials (if EEPROM is empty)
#define HARD_SSID "YourSSID"
#define HARD_PASS "YourPassword"

#define ServerURL "http://192.168.8.151:3000"
String storedMAC = "";

void reset() {
  // Save the new credentials to EEPROM
  for (int i = 0; i < MAX_SSID_LEN; i++) {
    EEPROM.write(EEPROM_SSID_ADDR + i, HARD_SSID[i]);
  }
  for (int i = 0; i < MAX_PASS_LEN; i++) {
    EEPROM.write(EEPROM_PASS_ADDR + i, HARD_PASS[i]);
  }
  EEPROM.commit();

  ESP.restart();
}

// ## Dont use D4/GPIO2, as its reserved for LED_BUILTIN
Button btn1(5, 5000);
Button btn2(0, 2000);

Blinker blinker(14);


// int redPin = 14;
int greenPin = 12;
int bluePin = 13;

void setup() {
  blinker.setupBlinker();
  // ######################### Do not change this Code ###########################################
  Serial.begin(115200);
  wifiSetupOTA();
  initSerialWebLogger();
  // #############################################################################################
  EEPROM.begin(EEPROM_SIZE);
  setupSensor(4);
  tempSensorBegin();
  setResolution(12);

  setupHTTP("http://192.168.8.151:3000");

  
  for (int i = 0; i < MAX_MAC_LEN; i++)
  {
    char c = EEPROM.read(EEPROM_MAC_ADDR + i);
    if (c == '\0' || c == (char)0xFF)       // reached the terminator
      break;
    storedMAC += c; 
  }

  btn1.begin();
  btn2.begin();
  // btn3.begin();

  // pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  String res = HTTPGET("/test");
  if(res != R"({"message":"connection successful"})"){
    digitalWrite(bluePin, LOW);
  }
}
bool RED = 1;
bool GREEN = 1;
bool BLUE = 1;

long t = millis();
int interval = 2;

void loop(){
  // ######################### Do not change this Code ###########################################
  otaHandle();
  updateSerialWebLogger();
  // #############################################################################################


  btn1.update();
  btn2.update();

  if (btn1.isLongPressed()) {
    reset();
  }

  if (millis() - t > interval * 1000) {
    int tries = 3;
    String res = "";
    while(res != R"({"status":"ok"})" && tries >0){
      tries--;
      res = HTTPPOST(String("/api/set/temp/")+String(storedMAC), String("{\"Temp\":"+String(readTempSensor(), 2))+String("}"));
    }
    if(res!=R"({"status":"ok"})"){
      digitalWrite(greenPin, LOW);
    }else{
      digitalWrite(greenPin, HIGH);
    }
    logMessage(readTempSensor());



    t = millis();
  }

  blinker.blink(1000);
}
