// ######################### Do not change this Code ###########################################
#include "src/WIFI_Programmer.h"
#include "src/SerialWebLogger.h"
// #############################################################################################
#include "src/DigitalTempSensor.h"
#include "src/Blinker.h"
#include "src/Button.h"
#include "src/HTTPRequest.h"

#include <EEPROM.h>
#include <ArduinoJson.h>
#include <pgmspace.h>

/* ------------------- EEPROM layout & constants ------------------- */
#define EEPROM_SIZE          160
#define EEPROM_SSID_ADDR       0
#define EEPROM_PASS_ADDR      32
#define EEPROM_UID_ADDR       96
#define EEPROM_MAC_ADDR      128
#define EEPROM_INTERVAL_ADDR 146

#define MAX_SSID_LEN    32
#define MAX_PASS_LEN    64
#define MAX_UID_LEN     32
#define MAX_MAC_LEN     18

/* defaults */
#define HARD_SSID     "YourSSID"
#define HARD_PASS     "YourPassword"
#define HARD_UID      "YourUID"
#define HARD_INTERVAL 86400          // 24 h seconds

/* server */
#define SERVER_URL    "http://192.168.8.151:3000"

/* ------------------- global state ------------------- */
String storedMAC;                    // filled in setup
int    INTERVAL = 10;                 // will be updated from server

// Blinker blinker(14);
Button  btn1(5, 5000);               // long‑press reset
Button  btn2(0, 100);

StaticJsonDocument<256> jdoc;        // reused buffer

/* ------------------- utility ------------------- */
inline void eepromWriteIfDiff(int addr, char value)
{
  if (EEPROM.read(addr) != value) EEPROM.write(addr, value);
}

static bool waitWiFi(uint8_t retries = 20, uint16_t stepMs = 500)
{
  while (WiFi.status() != WL_CONNECTED && retries--) {
    delay(stepMs);
  }
  return WiFi.status() == WL_CONNECTED;
}

static void factoryReset()
{
  for (int i = 0; i < MAX_SSID_LEN;  ++i) eepromWriteIfDiff(EEPROM_SSID_ADDR + i, HARD_SSID[i]);
  for (int i = 0; i < MAX_PASS_LEN;  ++i) eepromWriteIfDiff(EEPROM_PASS_ADDR + i, HARD_PASS[i]);
  EEPROM.commit();
  ESP.restart();
}

/* ------------------- setup ------------------- */
void setup()
{ 

  pinMode(12, OUTPUT);   // green  LED
  pinMode(13, OUTPUT);   // blue   LED
  pinMode(14, OUTPUT);   // red   LED
  
  digitalWrite(12, HIGH);                     // blue off = fail
  digitalWrite(13, HIGH);                     // blue off = fail
  digitalWrite(14, HIGH);

  // blinker.setupBlinker();

  // ######################### Do not change this Code ###########################################
  Serial.begin(115200);
  wifiSetupOTA();
  initSerialWebLogger();
  // #############################################################################################

  EEPROM.begin(EEPROM_SIZE);

  setupSensor(4);
  tempSensorBegin();
  setResolution(12);

  setupHTTP(F(SERVER_URL));

  /* build storedMAC from EEPROM */
  storedMAC.reserve(MAX_MAC_LEN);
  for (int i = 0; i < MAX_MAC_LEN; ++i) {
    char c = EEPROM.read(EEPROM_MAC_ADDR + i);
    if (c == '\0' || c == (char)0xFF) break;
    storedMAC += c;
  }

  btn1.begin();
  btn2.begin();

  /* quick server self‑test */
  String res = HTTPGET(F("/test"));
  deserializeJson(jdoc, res);
  if (jdoc["message"] != "connection successful") {
    digitalWrite(13, LOW);                     // blue off = fail
  } else if (jdoc.containsKey("interval")) {
    int eepInterval;
    EEPROM.get(EEPROM_INTERVAL_ADDR, eepInterval);
    if (eepInterval != jdoc["interval"].as<int>()) {
      EEPROM.put(EEPROM_INTERVAL_ADDR, jdoc["interval"].as<int>());
      EEPROM.commit();
    }
    INTERVAL = jdoc["interval"].as<int>();
  }
  delay(2000);
}

/* ------------------- loop ------------------- */
long lastSend = millis();
bool programMode = false;
void loop()
{
  // ######################### Do not change this Code ###########################################
  otaHandle();
  updateSerialWebLogger();
  // #############################################################################################

  btn1.update();
  btn2.update();

  if (btn1.isLongPressed()) factoryReset();

  /* timed POST */
  if (millis() - lastSend > (long)INTERVAL * 1000) {

    /* POST body → {"Temp":32.05} */
    String body;
    body.reserve(32);
    body = F("{\"Temp\":");
    body += String(readTempSensor(), 2);
    body += '}';

    /* endpoint /api/set/temp/<MAC> */
    String endpoint = F("/api/set/temp/");
    endpoint += storedMAC;

    /* retry up to 3 */
    bool success = false;
    for (uint8_t t = 0; t < 3 && !success; ++t) {
      String reply = HTTPPOST(endpoint, body);
      if (!deserializeJson(jdoc, reply) && jdoc["status"] == "ok") success = true;
    }

    if (!success) {
      digitalWrite(12, LOW);                   // green off = error
    } else {
      digitalWrite(12, HIGH);

      /* update interval if sent */
      if (jdoc.containsKey("interval")) {
        INTERVAL = jdoc["interval"].as<int>();
        EEPROM.put(EEPROM_INTERVAL_ADDR, INTERVAL);
        EEPROM.commit();
      }

      /* update Wi‑Fi creds if sent */
      if (jdoc.containsKey("SSID")) {
        String newSSID   = jdoc["SSID"    ];
        String newPass   = jdoc["Password"];
        for (int i = 0; i < MAX_SSID_LEN; ++i) eepromWriteIfDiff(EEPROM_SSID_ADDR + i, newSSID[i]);
        for (int i = 0; i < MAX_PASS_LEN; ++i) eepromWriteIfDiff(EEPROM_PASS_ADDR + i, newPass[i]);
        EEPROM.commit();
        ESP.restart();
      }
    }

    logMessage(readTempSensor());
    lastSend = millis();
  }
  // blinker.blink(1000);
  if(btn2.isLongPressed() && !programMode){
    digitalWrite(14, LOW);
    programMode = !programMode;
  }
  if(!programMode){
    ESP.deepSleep(INTERVAL*1000000);
  }
}
