#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <Arduino.h>

// Call once in setup(), passing the base URL (e.g. "https://api.example.com").
void setupHTTP(const String& baseURL);

// Perform an HTTP GET to baseURL + endpoint, returns payload or error string.
String HTTPGET(const String& endpoint = "");

// (Optional) Perform an HTTP POST; returns response or error string.
String HTTPPOST(const String& endpoint, const String& payload);

#endif // HTTPREQUEST_H
