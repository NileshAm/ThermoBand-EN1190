#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <Arduino.h>

// Call once in setup() ― pass the full base URL, e.g.
//   "http://192.168.8.151:3000"     (plain)
//   "https://api.example.com"       (TLS)
void setupHTTP(const String& baseURL, bool ignoreTLS = true);

// GET  baseURL + endpoint  → returns payload or "ERR: …"
String HTTPGET(const String& endpoint = "");

// POST baseURL + endpoint with payload  → returns response or "ERR: …"
String HTTPPOST(const String& endpoint,
                const String& payload,
                const String& contentType = "application/json");

#endif  // HTTPREQUEST_H
