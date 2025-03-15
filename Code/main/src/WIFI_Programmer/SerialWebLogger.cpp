#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include "SerialWebLogger.h"

// Create a WebSockets server on port 81.
WebSocketsServer webSocket = WebSocketsServer(81);

// Buffer to store log messages (if you need to keep history).
String serialBuffer = "";

// WebSocket event handler.
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      // Send an empty string on connection, so clients don't receive the full history.
      webSocket.sendTXT(num, "");
      break;
    }
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected\n", num);
      break;
    default:
      break;
  }
}

void initSerialWebLogger() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket logger started on port 81");
}

void updateSerialWebLogger() {
  webSocket.loop();
}

// logSerialMessage prints the message to Serial and then broadcasts only the new message.
void logMessage(const String &msg) {
  Serial.println(msg);
  serialBuffer += msg + "\n";
  String tmp = msg;  // temporary copy that is non-const
  webSocket.broadcastTXT(tmp);
}
void logMessage(const char* msg) {
  logMessage(String(msg));
}

// Overload for int.
void logMessage(int msg) {
  logMessage(String(msg));
}

// Overload for float.
void logMessage(float msg) {
  logMessage(String(msg));
}

// Overload for double.
void logMessage(double msg) {
  logMessage(String(msg));
}

void logMessage(unsigned long msg) {
  logMessage(String(msg));
}

