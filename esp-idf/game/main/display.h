#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void initDisplay();

void lcdMessage(const char* line1, const char* line2);
void lcdMessage(String line1, String line2);

void lcdClear();

#endif