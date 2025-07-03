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
#define MAX_SSID_LEN    100
#define MAX_PASS_LEN    100
#define MAX_UID_LEN     37
#define MAX_MAC_LEN     18

#define EEPROM_SSID_ADDR       0
#define EEPROM_PASS_ADDR      (EEPROM_SSID_ADDR + MAX_SSID_LEN)
#define EEPROM_UID_ADDR       (EEPROM_PASS_ADDR + MAX_PASS_LEN)
#define EEPROM_MAC_ADDR       (EEPROM_UID_ADDR + MAX_UID_LEN)
#define EEPROM_INTERVAL_ADDR  (EEPROM_MAC_ADDR + MAX_MAC_LEN)
#define EEPROM_SIZE           (EEPROM_INTERVAL_ADDR + 4)


/* defaults */
#define HARD_SSID     "YourSSID"
#define HARD_PASS     "YourPassword"
#define HARD_UID      "YourUID"
#define HARD_INTERVAL 300          // 5mins in seconds

/* server */
#define SERVER_URL    "https://thermoband-production.up.railway.app"

/* ------------------- global state ------------------- */
String storedMAC;                    // filled in setup
int    INTERVAL = HARD_INTERVAL;                 // will be updated from server

// Blinker blinker(14);
Button  btn1(5, 100);               // long‑press reset
Button  btn2(0, 50);

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

float poly_func(float x) {
    // Coefficients for order 4 polynomial
    float c[] = {-0.0013469376, 0.2189753045, -13.2929114574, 357.3340705466, -3554.4054571583};
    float value = 0;
    for (int i = 0; i < 5; i++) {
        value += c[i] * pow(x, 4 - i);
    }
    return value;
}

/* ------------------- setup ------------------- */
void setup()
{ 

  pinMode(12, OUTPUT);   // green  LED
  pinMode(13, OUTPUT);   // blue   LED
  pinMode(14, OUTPUT);   // red   LED
  
  digitalWrite(12, LOW);                    
  digitalWrite(13, HIGH);                     
  digitalWrite(14, HIGH);

  // blinker.setupBlinker();

  // ######################### Do not change this Code ###########################################
  Serial.begin(115200);
  wifiSetupOTA();
  initSerialWebLogger();
  // #############################################################################################
  btn1.begin();
  btn2.begin();

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


  /* quick server self‑test */
  String res = HTTPGET(F("/"));
  deserializeJson(jdoc, res);
  if (jdoc["message"] != "Health Monitoring Backend API is running") {
    digitalWrite(13, LOW);                     
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
bool programMode = false;
void loop()
{
  btn1.update();
  btn2.update();
  // ######################### Do not change this Code ###########################################
  otaHandle();
  updateSerialWebLogger();
  // #############################################################################################


  
  // Serial.println("Button state");
  // delay(200);
  Serial.println(btn2.isLongPressed());
  if(btn2.isLongPressed() && !programMode){
    digitalWrite(14, LOW);
    digitalWrite(12, HIGH);
    programMode = !programMode;
  }
  if(!programMode){
    /* timed POST */

    StaticJsonDocument<96> doc;
    doc["temperature"] = poly_func(readTempSensor());
    doc["macAddress"] = storedMAC;

    String body;
    serializeJson(doc, body);

    String endpoint = F("/esp32/add-temperature");

    /* retry up to 3 */
    bool success = false;
    Serial.print("Sending Post request");
    while (!success) {
      Serial.print(".");
      String reply = HTTPPOST(endpoint, body);
      if (!deserializeJson(jdoc, reply) && jdoc["message"] == "Temperature recorded"){
        float t = jdoc["tempRecord"]["out_temperature"].as<float>();
        Serial.println(t);
        success = true;
      }else{
        digitalWrite(14, LOW);                   // green off = error
      }
    }
    Serial.println("#");
    digitalWrite(14, HIGH);                   // green off = error

    if (jdoc.containsKey("interval")) {
      INTERVAL = jdoc["interval"].as<int>();
      EEPROM.put(EEPROM_INTERVAL_ADDR, INTERVAL);
      EEPROM.commit();
    }
    Serial.println(jdoc.containsKey("reset"));
    Serial.println(jdoc["reset"].as<bool>());
    if (jdoc.containsKey("reset") && jdoc["reset"].as<bool>()) {
      factoryReset();
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

    digitalWrite(12, HIGH);
    // blinker.blink(1000);
    Serial.println("sleep");
    ESP.deepSleep(INTERVAL*1000000);
  }
}
