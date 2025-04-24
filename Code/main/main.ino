// ######################### Do not change this Code ###########################################
#include "src/WIFI_Programmer/WIFI_Programmer.h"
#include "src/WIFI_Programmer/SerialWebLogger.h"
// #############################################################################################
#include "src/DigitalTempSensor/DigitalTempSensor.h"
#include "src/Blinker/Blinker.h"
#include "src/Button/Button.h"

#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClient.h>

// EEPROM configuration
#define EEPROM_SIZE 128
#define EEPROM_SSID_ADDR 0
#define EEPROM_PASS_ADDR 32
#define EEPROM_UID_ADDR 96
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define MAX_UID_LEN 32

// Hardcoded default credentials (if EEPROM is empty)
#define HARD_SSID "YourSSID"
#define HARD_PASS "YourPassword"

#define ServerURL "https://www.omdbapi.com/?apikey=a5e60159&s=spider"

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
<<<<<<< Updated upstream
// Button btn3(10, 2000);
=======
<<<<<<< Updated upstream
// Button btn3(16, 2000);
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes

Blinker blinker(14);


// int redPin = 14;
int greenPin = 12;
int bluePin = 13;

String SSID = "";
String Pass = "";
String UID = "";

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


  btn1.begin();
  btn2.begin();
  // btn3.begin();

  // pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  // pinMode(2, OUTPUT);
  // digitalWrite(2, HIGH);

  for (int i = 0; i < MAX_SSID_LEN; i++) {
    SSID += char(EEPROM.read(EEPROM_SSID_ADDR + i));
  }
  for (int i = 0; i < MAX_PASS_LEN; i++) {
    Pass += char(EEPROM.read(EEPROM_PASS_ADDR + i));
  }
  for (int i = 0; i < MAX_UID_LEN; i++) {
    UID += char(EEPROM.read(EEPROM_UID_ADDR + i));
  }
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

  if (btn1.isLongPressed()) {
    // BLUE = !BLUE;
    // // digitalWrite(redPin, RED);
    // digitalWrite(greenPin, GREEN);
    // digitalWrite(bluePin, BLUE);
    // logMessage("btn1");
    reset();
  }
  if (btn2.wasShortPress()) {
    GREEN = !GREEN;
    // digitalWrite(redPin, RED);
    digitalWrite(greenPin, GREEN);
    digitalWrite(bluePin, BLUE);
    logMessage("btn2");
  }
  // if (btn3.wasShortPress()) {
  //   BLUE = !BLUE;
  //   digitalWrite(redPin, RED);
  //   digitalWrite(greenPin, GREEN);
  //   digitalWrite(bluePin, BLUE);
  //   logMessage("btn3");
  // }

  if (millis() - t > interval * 1000) {
    // digitalWrite(redPin, LOW);
    lastRead = millis();
    logMessage(readTempSensor());
    // logMessage(SSID);
    // logMessage(Pass);
    // logMessage(UID);

    // std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    // // client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    // client->setInsecure();

    // HTTPClient https;
    // logMessage("http");
    // if (https.begin(*client, ServerURL)) {  // HTTP
    //   // start connection and send HTTP header
    //   int httpCode = https.GET();
    //   // httpCode will be negative on error
    //   if (httpCode > 0) {
    //     // HTTP header has been send and Server response header has been handled
    //     logMessage("[HTTP] GET... code: ");
    //     logMessage(httpCode);
    //     logMessage("\n");

    //       // file found at server
    //     String payload = https.getString();
    //     logMessage(payload);
    //     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    //       String payload = https.getString();
    //       logMessage(payload);
    //     }
    //   } else {
    //     Serial.printf("[HTTP] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    //   }

    //   https.end();
    // } else {
    //   logMessage("[HTTP] Unable to connect");
    // }


    t = millis();
  }

  if (millis() - lastRead > 500) {
    // digitalWrite(redPin, HIGH);
  }

  blinker.blink(1000);
}
