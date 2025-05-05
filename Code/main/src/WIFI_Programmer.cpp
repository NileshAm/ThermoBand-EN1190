#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "WIFI_Programmer.h"
#include "HTTPRequest.h"

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
#define HARD_UID "YourUID"

// Create a web server on port 80 (for configuration portal)
ESP8266WebServer server(80);

//---------------------------
// Web server handlers for AP configuration
//---------------------------
void handleRoot()
{
  String html = R"(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ESP8266 WiFi Setup</title>
    <style>
        .flex{
            display: flex;
        }
        .jestify-content-center{
            justify-content: center;
        }
        .jestify-content-around{
            justify-content: space-around;
        }
        .align-items-center{
            align-items: center;
        }
        .flex-col{
            flex-direction: column;
        }
        .w-100{
            width: 100%;
        }
        .w-50{
            width: 50%;
        }
        .w-25{
            width: 25%;
        }
        .w-75{
            width: 75%;
        }
        .red{
            background-color: red;
        }
        .text-align-center{
            text-align: center;
        }
        .p-2{
            padding: 0.2rem;
        }
        .p-4{
            padding: 0.8rem;
        }
        .m-3{
            margin: 0.3rem;
        }
        .mt-3{
            margin-top: 0.3rem;
        }
        .form{
            width: 50%;
            background-color: rgb(235, 235, 235);
            border-radius: 0.75rem;
            border: solid 1px rgb(192, 192, 192);
            box-shadow: 0.1rem 0.1rem 0.2rem rgb(214, 213, 213);
        }
    </style>
  </head>
  <body class=" flex jestify-content-center">
      <form action="/save" method="POST" class=" form flex flex-col align-items-center justify-content-around p-4 mt-3">
        <h1 class="w-100 text-align-center">Enter Credentials</h1>
        <div class="flex jestify-content-around w-100 p-2">
            <label for="ssid" class="w-25">SSID: </label>
            <input type="text" name="ssid"  class="w-75"/>
        </div>
        <div class="flex jestify-content-around w-100 p-2">
            <label for="pass" class="w-25">Password: </label>
            <input type="password" name="pass" class="w-75"/>
        </div>
        <div class="flex jestify-content-around w-100 p-2">
            <label for="UID" class="w-25">UID: </label>
            <input type="text" name="UID" class="w-75"/>
        </div>
      <input type="submit" value="Save"  class="p-2 m-3 w-50"/>
    </form>
  </body>
</html>)";
  server.send(200, "text/html", html);
}

void handleSave()
{
  String newSSID = server.arg("ssid");
  String newPass = server.arg("pass");
  String newUID = server.arg("UID");

  Serial.println("New Credentials Received:");
  Serial.println("SSID: " + newSSID);
  Serial.println("Pass: " + newPass);
  Serial.println("UID: " + newUID);

  String html = R"(
      <html>
      <head>
      <title>Save</title>
      </head>
      <body>
      <h1>Credentials Saved</h1>
      <p>Rebooting...</p>
      <p>Redirecting in <span id="Time"></span> ......</p>
      </body>
      <script>
      time = 5;
      document.getElementById("Time").innerHTML = time;
      var interval = setInterval(function () {
        time--;
        document.getElementById("Time").innerHTML = time;
        if (time <= 0) {
          clearInterval(interval);
          window.location.href = "/";
          }
          }, 1000); // Update every second
          </script>
          </html>
          )";
  String WIFIFailed = R"(
            <html>
            <head>
            <title>WIFI Connection Failed</title>
            </head>
            <body>
            <h1>Unable to save credentials</h1>
            <h3>Maximum connection retries reached</h3>
            <p>Credentials might be incorrect</p>
            <p>Redirecting to Setup in <span id="Time" .....></span></p>
            <br>
            <button type="button" onclick="window.location.href = '/';">Redirect now</button>
            </body>
            <script>
            time = 10;
            document.getElementById("Time").innerHTML = time;
            var interval = setInterval(function () {
              time--;
              document.getElementById("Time").innerHTML = time;
              if (time <= 0) {
                clearInterval(interval);
                window.location.href = "/";
                }
                }, 1000); // Update every second
                </script>
                </html>)";

  String ServerFailed = R"(<html>
  <head>
    <title>Server Connection Failed</title>
  </head>
  <body>
    <h1>Unable to save credentials</h1>
    <h3>Unable to connect to server</h3>
    <p>Internet connection might be faulty, or invalid UID</p>
    <p>Redirecting to Setup in <span id="Time" .....></span></p>
    <br>
    <button type="button" onclick="window.location.href = '/';">Redirect now</button>
  </body>
  <script>
    time = 10;
    document.getElementById("Time").innerHTML = time;
    var interval = setInterval(function () {
      time--;
      document.getElementById("Time").innerHTML = time;
      if (time <= 0) {
        clearInterval(interval);
        window.location.href = "/";
      }
    }, 1000); // Update every second
  </script>
</html>)";
  WiFi.begin(newSSID.c_str(), newPass.c_str());

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    server.send(200, "text/html", WIFIFailed);
    ESP.restart();
  }

  setupHTTP("http://192.168.8.151:3000");
  int tries = 3;
  String res = "";
  while(res != R"({"status":"ok"})" && tries >0){
    tries--;
    res = HTTPPOST(String("/api/set/hid"), String("{\"UID\":\"") + String(newUID)+String("\",\"MacAddress\":\"") + String(WiFi.macAddress()) +String("\"}"));
  }
  if(res!=R"({"status":"ok"})")
  {
    server.send(200, "text/html", ServerFailed);
    ESP.restart();
  }

  // Save the new credentials to EEPROM
  for (int i = 0; i < MAX_SSID_LEN; i++)
  {
    EEPROM.write(EEPROM_SSID_ADDR + i, newSSID[i]);
  }
  for (int i = 0; i < MAX_PASS_LEN; i++)
  {
    EEPROM.write(EEPROM_PASS_ADDR + i, newPass[i]);
  }
  for (int i = 0; i < MAX_UID_LEN; i++)
  {
    EEPROM.write(EEPROM_UID_ADDR + i, newUID[i]);
  }
  EEPROM.commit();

  server.send(200, "text/html", html);

  // Attempt connection with new credentials then reboot
  delay(5000);
  ESP.restart();
}

void startAPMode()
{
  // Switch to AP mode with a fixed SSID for configuration
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266_Config");
  Serial.print("Config AP started, IP: ");
  Serial.println(WiFi.softAPIP());

  // Set up server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
  Serial.println("HTTP server started");
}

//---------------------------
// WiFi & OTA Setup function
//---------------------------
void wifiSetupOTA(const char *newSSID, const char *newPass, uint8_t LED_PIN)
{
  EEPROM.begin(EEPROM_SIZE);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  char storedSSID[MAX_SSID_LEN];
  char storedPass[MAX_PASS_LEN];
  char storedUID[MAX_UID_LEN];
  bool updateEEPROM = false;

  // If new credentials are provided, use them and mark to update EEPROM.
  if (newSSID != NULL && newPass != NULL && strlen(newSSID) > 0 && strlen(newPass) > 0)
  {
    strncpy(storedSSID, newSSID, MAX_SSID_LEN - 1);
    storedSSID[MAX_SSID_LEN - 1] = '\0';
    strncpy(storedPass, newPass, MAX_PASS_LEN - 1);
    storedPass[MAX_PASS_LEN - 1] = '\0';
    updateEEPROM = true;
  }
  else
  {
    // Otherwise, load credentials from EEPROM.
    for (int i = 0; i < MAX_SSID_LEN; i++)
    {
      storedSSID[i] = char(EEPROM.read(EEPROM_SSID_ADDR + i));
    }
    storedSSID[MAX_SSID_LEN - 1] = '\0';
    for (int i = 0; i < MAX_PASS_LEN; i++)
    {
      storedPass[i] = char(EEPROM.read(EEPROM_PASS_ADDR + i));
    }
    storedPass[MAX_PASS_LEN - 1] = '\0';

    // If EEPROM has no valid data, fall back to defaults.
    if (strlen(storedSSID) == 0 || strlen(storedPass) == 0)
    {
      strncpy(storedSSID, HARD_SSID, MAX_SSID_LEN - 1);
      storedSSID[MAX_SSID_LEN - 1] = '\0';
      strncpy(storedPass, HARD_PASS, MAX_PASS_LEN - 1);
      storedPass[MAX_PASS_LEN - 1] = '\0';
      updateEEPROM = true;
    }
  }

  // Update EEPROM if necessary.
  if (updateEEPROM)
  {
    for (int i = 0; i < MAX_SSID_LEN; i++)
    {
      EEPROM.write(EEPROM_SSID_ADDR + i, storedSSID[i]);
    }
    for (int i = 0; i < MAX_PASS_LEN; i++)
    {
      EEPROM.write(EEPROM_PASS_ADDR + i, storedPass[i]);
    }
    for (int i = 0; i < MAX_UID_LEN; i++)
    {
      EEPROM.write(EEPROM_UID_ADDR + i, storedUID[i]);
    }
    EEPROM.commit();
  }

  // Attempt connection in STA mode.
  WiFi.mode(WIFI_STA);
  // Read MAC address from EEPROM
  String mac = WiFi.macAddress();          // "AA:BB:CC:DD:EE:FF"
  char buf[MAX_MAC_LEN];
  mac.toCharArray(buf, MAX_MAC_LEN);

  for (uint8_t i = 0; i < MAX_MAC_LEN; ++i)
    EEPROM.write(EEPROM_MAC_ADDR + i, buf[i]);
  EEPROM.commit();
  
  WiFi.begin(storedSSID, storedPass);

  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());

    // Initialize OTA updates
    ArduinoOTA.onStart([]()
                       {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";
      Serial.println("Start updating " + type); });
    ArduinoOTA.onEnd([]()
                     { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\n", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
    ArduinoOTA.setPassword("230038E");
    ArduinoOTA.begin();
    Serial.println("OTA Ready");

    // Proceed normally (OTA updates can now be uploaded via WiFi)
  }
  else
  {
    Serial.println();
    Serial.println("Failed to connect. Switching to AP mode for configuration.");
    startAPMode();
    // Block here handling client requests until configuration.
    int ledState = LOW;

    unsigned long previousMillis = 0;
    const long interval = 100;
    while (true)
    {
      server.handleClient();
      // delay(10);
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
        previousMillis = currentMillis;
        if (ledState == LOW)
        {
          ledState = HIGH; // Note that this switches the LED *off*
        }
        else
        {
          ledState = LOW; // Note that this switches the LED *on*
        }
        digitalWrite(LED_PIN, ledState);
      }
    }
  }
}

//---------------------------
// OTA handling function (to be called in loop)
//---------------------------
void otaHandle()
{
  ArduinoOTA.handle();
}
