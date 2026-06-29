#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

void connectWiFi();
bool isWiFiConnected();
void reconnectWiFi();

#endif