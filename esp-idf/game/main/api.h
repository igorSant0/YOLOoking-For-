#ifndef API_H
#define API_H

#include <ArduinoJson.h>

bool connectServer();

bool gameInit();

bool validateObject();

bool nextHint();

bool resetGame();

bool sendGET(String endpoint);

void processServerResponse(JsonDocument &doc);

#endif