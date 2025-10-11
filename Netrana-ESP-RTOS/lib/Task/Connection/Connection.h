#ifndef CONNECTION_H
#define CONNECTION_H

#include <WiFi.h>
#include <Preferences.h>
#include "time.h"

// Use String instead of const char* to avoid dangling pointers
extern String wifiSSID;
extern String wifiPassword;

extern const char *serverHost;
extern const int   serverPort;
extern const char *serverPath;
extern const char *controlPath;
extern const char *emergencyHost;

void initWiFi();
void connectToWiFi();
void setupTime();
bool isWiFiConnected();

#endif