#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <WiFi.h>
#include <WebServer.h>

// --- The Extern Bridge ---
// These let your web handler know that the variables exist over in robodog.ino
extern volatile float targetX;
extern volatile float targetY;
extern volatile float targetZ;
extern volatile float presentX;
extern volatile float presentY;
extern volatile float presentZ;
extern volatile int robotMode; 
extern WebServer server; 
extern unsigned long webStepStartTime; 

// --- "Just It Exists" Function Blueprints ---
void setupWiFi();
void handleRoot();
void handleChangeMode();
void handleMove();
void handleSet();

#endif