#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "WIFI_Programmer.h"

// EEPROM configuration
#define EEPROM_SIZE       96
#define EEPROM_SSID_ADDR  0
#define EEPROM_PASS_ADDR  32
#define MAX_SSID_LEN      32
#define MAX_PASS_LEN      64

// Hardcoded default credentials (if EEPROM is empty)
#define HARD_SSID "YourSSID"
#define HARD_PASS "YourPassword"

// Create a web server on port 80 (for configuration portal)
ESP8266WebServer server(80);

//---------------------------
// Web server handlers for AP configuration
//---------------------------
void handleRoot() {
  String html = "<html><head><title>ESP8266 WiFi Setup</title></head><body>";
  html += "<h1>Enter WiFi Credentials</h1>";
  html += "<form action='/save' method='POST'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='pass'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  String newSSID = server.arg("ssid");
  String newPass = server.arg("pass");
  
  Serial.println("New Credentials Received:");
  Serial.println("SSID: " + newSSID);
  Serial.println("Pass: " + newPass);
  
  server.send(200, "text/html", "<html><body><h1>Credentials Saved</h1><p>Rebooting...</p></body></html>");
  delay(1000);
  
  // Save the new credentials to EEPROM
  for (int i = 0; i < MAX_SSID_LEN; i++) {
    EEPROM.write(EEPROM_SSID_ADDR + i, newSSID[i]);
  }
  for (int i = 0; i < MAX_PASS_LEN; i++) {
    EEPROM.write(EEPROM_PASS_ADDR + i, newPass[i]);
  }
  EEPROM.commit();
  
  // Attempt connection with new credentials then reboot
  WiFi.begin(newSSID.c_str(), newPass.c_str());
  delay(5000);
  ESP.restart();
}

void startAPMode() {
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
void wifiSetupOTA(const char* newSSID, const char* newPass) {
  EEPROM.begin(EEPROM_SIZE);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  char storedSSID[MAX_SSID_LEN];
  char storedPass[MAX_PASS_LEN];
  bool updateEEPROM = false;

  // If new credentials are provided, use them and mark to update EEPROM.
  if (newSSID != NULL && newPass != NULL && strlen(newSSID) > 0 && strlen(newPass) > 0) {
    strncpy(storedSSID, newSSID, MAX_SSID_LEN - 1);
    storedSSID[MAX_SSID_LEN - 1] = '\0';
    strncpy(storedPass, newPass, MAX_PASS_LEN - 1);
    storedPass[MAX_PASS_LEN - 1] = '\0';
    updateEEPROM = true;
  } else {
    // Otherwise, load credentials from EEPROM.
    for (int i = 0; i < MAX_SSID_LEN; i++) {
      storedSSID[i] = char(EEPROM.read(EEPROM_SSID_ADDR + i));
    }
    storedSSID[MAX_SSID_LEN - 1] = '\0';
    for (int i = 0; i < MAX_PASS_LEN; i++) {
      storedPass[i] = char(EEPROM.read(EEPROM_PASS_ADDR + i));
    }
    storedPass[MAX_PASS_LEN - 1] = '\0';
    
    // If EEPROM has no valid data, fall back to defaults.
    if (strlen(storedSSID) == 0 || strlen(storedPass) == 0) {
      strncpy(storedSSID, HARD_SSID, MAX_SSID_LEN - 1);
      storedSSID[MAX_SSID_LEN - 1] = '\0';
      strncpy(storedPass, HARD_PASS, MAX_PASS_LEN - 1);
      storedPass[MAX_PASS_LEN - 1] = '\0';
      updateEEPROM = true;
    }
  }

  // Update EEPROM if necessary.
  if (updateEEPROM) {
    for (int i = 0; i < MAX_SSID_LEN; i++) {
      EEPROM.write(EEPROM_SSID_ADDR + i, storedSSID[i]);
    }
    for (int i = 0; i < MAX_PASS_LEN; i++) {
      EEPROM.write(EEPROM_PASS_ADDR + i, storedPass[i]);
    }
    EEPROM.commit();
  }

  // Attempt connection in STA mode.
  WiFi.mode(WIFI_STA);
  WiFi.begin(storedSSID, storedPass);
  
  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    
    // Initialize OTA updates
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    
    // Proceed normally (OTA updates can now be uploaded via WiFi)
  } else {
    Serial.println();
    Serial.println("Failed to connect. Switching to AP mode for configuration.");
    startAPMode();
    // Block here handling client requests until configuration.
    int ledState = LOW;

    unsigned long previousMillis = 0;
    const long interval = 100;
    while (true) {
      server.handleClient();
      // delay(10);
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;  // Note that this switches the LED *off*
        } else {
          ledState = LOW;  // Note that this switches the LED *on*
        }
        digitalWrite(LED_BUILTIN, ledState);
      }
    }
  }
}

//---------------------------
// OTA handling function (to be called in loop)
//---------------------------
void otaHandle() {
  ArduinoOTA.handle();
}
