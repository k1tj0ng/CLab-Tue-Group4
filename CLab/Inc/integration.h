#ifndef INTEGRATION_H
#define INTEGRATION_H

#include "serial.h"

extern int INTERVAL_BUFFER;

void handleNumericCommand(const char* name, const char* value);

void handleSerial(const char* value);

void sortingOutInput(char buffers[][BUFFER], uint8_t bufIndex);

#endif
