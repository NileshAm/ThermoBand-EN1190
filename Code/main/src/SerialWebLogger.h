#ifndef SERIAL_WEBLOGGER_H
#define SERIAL_WEBLOGGER_H

#include <WebSocketsServer.h>

// Initialize the WebSocket server (on port 81) for Serial logging.
void initSerialWebLogger();

// Process WebSocket events; call this in your loop().
void updateSerialWebLogger();

// Log a message: prints to Serial Monitor and sends it over WebSockets.
void logMessage(const String &msg);
void logMessage(const char* msg);
void logMessage(int msg);
void logMessage(float msg);
void logMessage(double msg);
void logMessage(unsigned long msg);

#endif // SERIAL_WEBLOGGER_H
