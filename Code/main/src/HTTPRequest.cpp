#include "HTTPRequest.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

// -------------------------
// globals
// -------------------------
static String              gBaseURL;
static bool                gUsesTLS = false;
static bool                gTLSInsecure = true;

static WiFiClient          gPlain;
static std::unique_ptr<BearSSL::WiFiClientSecure> gSecure;
static HTTPClient          gHTTP;

// -------------------------
// init
// -------------------------
void setupHTTP(const String& baseURL, bool ignoreTLS)
{
  gBaseURL = baseURL;
  gUsesTLS = baseURL.startsWith("https://");
  gTLSInsecure = ignoreTLS;

  if (gUsesTLS) {
    gSecure.reset(new BearSSL::WiFiClientSecure);
    if (gTLSInsecure) gSecure->setInsecure();           // skip cert check
  }
}

// -------------------------
// helpers
// -------------------------
static String beginRequest(const String& url)
{
  bool ok;
  if (gUsesTLS)
    ok = gHTTP.begin(*gSecure, url);
  else
    ok = gHTTP.begin(gPlain, url);

  if (!ok) return F("ERR: begin() failed");
  return String();     // empty == success
}

static String handleResult(int code)
{
  if (code <= 0) {
    String err = F("ERR: connection ") + String(code);
    gHTTP.end();
    return err;
  }
  if (code != HTTP_CODE_OK && code != HTTP_CODE_CREATED &&
      code != HTTP_CODE_MOVED_PERMANENTLY) {
    String err = F("ERR: HTTP ") + String(code);
    gHTTP.end();
    return err;
  }
  String payload = gHTTP.getString();
  gHTTP.end();
  return payload;
}

// -------------------------
// GET
// -------------------------
String HTTPGET(const String& endpoint)
{
  String url = gBaseURL + endpoint;
  String err = beginRequest(url);
  if (err.length()) return err;

  int code = gHTTP.GET();
  return handleResult(code);
}

// -------------------------
// POST
// -------------------------
String HTTPPOST(const String& endpoint,
                const String& payload,
                const String& contentType)
{
  String url = gBaseURL + endpoint;
  String err = beginRequest(url);
  if (err.length()) return err;

  gHTTP.addHeader(F("Content-Type"), contentType);
  int code = gHTTP.POST(payload);
  return handleResult(code);
}
