#ifndef WIFI_PROGRAMMER_H
#define WIFI_PROGRAMMER_H

// Call this from setup() to initialize WiFi and OTA.
// Optional newSSID and newPass parameters allow updating credentials.
void wifiSetupOTA(const char* newSSID = NULL, const char* newPass = NULL);

// Call this in your loop() to handle OTA updates.
void otaHandle();

#endif // WIFI_PROGRAMMER_H
