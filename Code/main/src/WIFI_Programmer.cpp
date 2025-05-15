/*  ───────────────────────────  deps  ─────────────────────────── */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

#include "WIFI_Programmer.h"
#include "HTTPRequest.h"

/*  ───────────────────── eeprom & constants  ───────────────────── */
#define EEPROM_SIZE          160        // must be ≤ 4096
#define EEPROM_SSID_ADDR       0
#define EEPROM_PASS_ADDR      32
#define EEPROM_UID_ADDR       96
#define EEPROM_MAC_ADDR      128
#define EEPROM_INTERVAL_ADDR 146

constexpr uint16_t  MAX_SSID_LEN = 32;
constexpr uint16_t  MAX_PASS_LEN = 64;
constexpr uint16_t  MAX_UID_LEN  = 32;
constexpr uint16_t  MAX_MAC_LEN  = 18;

/* defaults */
constexpr char HARD_SSID[]   = "YourSSID";
constexpr char HARD_PASS[]   = "YourPassword";
constexpr char HARD_UID[]    = "YourUID";
constexpr uint32_t HARD_INT  = 86'400UL;   // 24 h

/*  ───────────────────── static buffers  ──────────────────────── */
static char ssidBuf [MAX_SSID_LEN];
static char passBuf [MAX_PASS_LEN];
static char uidBuf  [MAX_UID_LEN ];
static char macBuf  [MAX_MAC_LEN ];
static uint32_t sendInterval;

/*  ───────────────────── forward decl  ────────────────────────── */
static void startAPMode(uint8_t ledPin);
static bool loadCredsFromEeprom();
static void saveCredsToEeprom();
static bool wifiConnect(uint8_t retries, uint16_t msStep);
static bool pushUIDtoServer();

/*  ───────────────────── html pages in flash  ─────────────────── */
static const char HTML_FORM[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP Wi‑Fi Setup</title>
<style>
*{box-sizing:border-box;font-family:sans-serif}body{display:flex;justify-content:center}
form{width:50%;background:#ebebeb;border-radius:12px;border:1px solid #c0c0c0;
box-shadow:2px 2px 5px #d6d5d5;padding:16px;margin-top:12px;display:flex;
flex-direction:column;align-items:center}
.w{width:75%}label{width:25%}.row{display:flex;width:100%;justify-content:space-around;padding:4px}
input[type=submit]{width:50%;padding:6px;margin:8px}
</style></head><body>
<form action="/save" method="POST">
  <h2>Enter credentials</h2>
  <div class="row"><label>SSID:</label><input class="w" name="ssid"></div>
  <div class="row"><label>Password:</label><input class="w" name="pass" type="password"></div>
  <div class="row"><label>UID:</label><input class="w" name="UID"></div>
  <input type="submit" value="Save">
</form></body></html>)rawliteral";


static const char HTML_OK[] PROGMEM = R"rawliteral(
<html><body><h1>Saved ‑ rebooting…</h1></body></html>)rawliteral";
static const char HTML_WIFI_FAIL[]   PROGMEM =
  "<html><body><h1>Wi‑Fi connect FAILED</h1></body></html>";
static const char HTML_SERVER_FAIL[] PROGMEM =
  "<html><body><h1>Server handshake FAILED</h1></body></html>";

/*  ───────────────────── web server  ──────────────────────────── */
ESP8266WebServer server(80);

static void handleRoot() { server.send_P(200, "text/html", HTML_FORM); }

// ───────────────── helper: send page → flush → reboot ───────────────
static void sendPageAndReboot(const char* htmlPROGMEM)
{
  server.send_P(200, "text/html", htmlPROGMEM);   // stream from flash
  server.client().flush();                        // push remaining bytes
  delay(1200);                                    // give browser time
  ESP.restart();                                  // single reboot point
}


static void handleSave()
{
  server.arg("ssid").toCharArray(ssidBuf, MAX_SSID_LEN);
  server.arg("pass").toCharArray(passBuf, MAX_PASS_LEN);
  server.arg("UID" ).toCharArray(uidBuf , MAX_UID_LEN );
  sendInterval = HARD_INT;

  
  if (!wifiConnect(20, 250)) {          // Wi‑Fi failed
    sendPageAndReboot(HTML_WIFI_FAIL);
  }
  if (!pushUIDtoServer()) {             // server handshake failed
    sendPageAndReboot(HTML_SERVER_FAIL);
  }
  saveCredsToEeprom();
  // success
  sendPageAndReboot(HTML_OK);

}

/*  ───────────────────── utilities  ───────────────────────────── */
static bool loadCredsFromEeprom()
{
  EEPROM.begin(EEPROM_SIZE);

  EEPROM.get(EEPROM_INTERVAL_ADDR, sendInterval);
  if (sendInterval == 0xFFFFFFFF) sendInterval = HARD_INT;

  EEPROM.get(EEPROM_SSID_ADDR, ssidBuf);          // reads null‑terminated
  EEPROM.get(EEPROM_PASS_ADDR, passBuf);
  EEPROM.get(EEPROM_UID_ADDR , uidBuf );

  if (ssidBuf[0] == '\0' || passBuf[0] == '\0') { // not initialised
    strncpy(ssidBuf, HARD_SSID, MAX_SSID_LEN);
    strncpy(passBuf, HARD_PASS, MAX_PASS_LEN);
    strncpy(uidBuf , HARD_UID , MAX_UID_LEN );
    return false;                                 // need to save defaults
  }
  return true;
}

static void saveCredsToEeprom()
{
  EEPROM.put(EEPROM_SSID_ADDR, ssidBuf);
  EEPROM.put(EEPROM_PASS_ADDR, passBuf);
  EEPROM.put(EEPROM_UID_ADDR , uidBuf );
  EEPROM.put(EEPROM_INTERVAL_ADDR, sendInterval);
  EEPROM.commit();
}

static bool wifiConnect(uint8_t retries, uint16_t msStep)
{
  // WiFi.mode(WIFI_STA);
  WiFi.begin(ssidBuf, passBuf);

  while (WiFi.status() != WL_CONNECTED && retries--) {
    delay(msStep);
  }
  return WiFi.status() == WL_CONNECTED;
}

static bool pushUIDtoServer()
{
  setupHTTP(F("http://192.168.8.151:3000"));
  StaticJsonDocument<96> doc;
  doc["UID"]        = uidBuf;
  doc["MacAddress"] = WiFi.macAddress();
  doc["Interval"]   = sendInterval;

  String body;
  serializeJson(doc, body);

  String res = HTTPPOST("/api/set/hid", body);
  deserializeJson(doc, res);
  return doc["status"] == "ok";
}

/*  ───────────────────── AP mode  ─────────────────────────────── */
static void startAPMode(uint8_t ledPin)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266_Config");
  server.on("/",     HTTP_GET,  handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();

  uint32_t t  = millis();
  bool led = false;
  while (true) {                                   // config portal loop
    server.handleClient();
    if (millis() - t > 150) {                      // blink ticker
      led = !led;
      digitalWrite(ledPin, led);
      t = millis();
    }
  }
}

/*  ───────────────────── public entry  ────────────────────────── */
void wifiSetupOTA(const char* overrideSSID,
                  const char* overridePASS,
                  uint8_t ledPin)
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  bool haveValid = loadCredsFromEeprom();
  if (overrideSSID && *overrideSSID) {             // caller overrides
    strncpy(ssidBuf, overrideSSID, MAX_SSID_LEN);
    strncpy(passBuf, overridePASS, MAX_PASS_LEN);
    haveValid = false;
  }
  if (!haveValid) saveCredsToEeprom();

  if (!wifiConnect(40, 250)) {                     // ~10 s total
    startAPMode(ledPin);                           // never returns
  }

  /* store MAC once */
  WiFi.macAddress().toCharArray(macBuf, MAX_MAC_LEN);
  EEPROM.put(EEPROM_MAC_ADDR, macBuf);
  EEPROM.commit();

  /* OTA */
  ArduinoOTA.setPassword("230038E");
  ArduinoOTA.begin();

  /* log */
  Serial.print(F("Wi‑Fi OK, IP=")); Serial.println(WiFi.localIP());
}

void otaHandle() { ArduinoOTA.handle(); }
