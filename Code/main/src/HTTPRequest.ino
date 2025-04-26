#include "HTTPRequest.h"

void setup() {
  Serial.begin(115200);
  // … connect WiFi …

  setupHTTP("https://api.example.com/");
  String data = HTTPGET("/status");
  Serial.println(data);
}

void loop() {
  // …
}
