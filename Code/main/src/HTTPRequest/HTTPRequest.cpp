#include "HTTPRequest.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Global state
static String serverURL;
static std::unique_ptr<BearSSL::WiFiClientSecure> secureClient;
static HTTPClient http;

void setupHTTP(const String& baseURL) {
  serverURL = baseURL;
  secureClient = std::make_unique<BearSSL::WiFiClientSecure>();
  secureClient->setInsecure();  // Skip certificate check
}

String HTTPGET(const String& endpoint) {
  if (!secureClient) return "Error: HTTP client not initialized";
  String fullURL = serverURL + endpoint;
  if (!http.begin(*secureClient, fullURL)) {
    return "Error: begin() failed";
  }

  int code = http.GET();
  if (code <= 0) {
    String err = "GET failed, error: " + String(code);
    http.end();
    return err;
  }

  String payload = http.getString();
  http.end();
  return payload;
}

String HTTPPOST(const String& endpoint, const String& payload) {
  if (!secureClient) return "Error: HTTP client not initialized";
  String fullURL = serverURL + endpoint;
  if (!http.begin(*secureClient, fullURL)) {
    return "Error: begin() failed";
  }
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payload);
  if (code <= 0) {
    String err = "POST failed, error: " + String(code);
    http.end();
    return err;
  }
  String response = http.getString();
  http.end();
  return response;
}
