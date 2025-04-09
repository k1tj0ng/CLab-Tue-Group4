#include "integration.h"

#include "stm32f303xc.h"
#include "serial.h"
#include "timer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int maxInputLength = 32;
int serialCommand = 0;

// Long int command handler
void handleNumericCommand(const char* name, const char* value) {
    if(value == NULL) {
        char msg[maxInputLength];
        sprintf(msg, "Error: %s needs value\n", name);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
        return;
    }

    char* endptr;
    long num = strtol(value, &endptr, 10);

    if(*endptr != '\0' || num < 0) {
        SerialOutputString((uint8_t*)"Error: Invalid number\n", &USART1_PORT);
    } else {
        char msg[maxInputLength];
        sprintf(msg, "%s: %ld\n", name, num);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
    }
}

// String command handler
void handleSerial(const char* value) {
    if(value == NULL) {
        SerialOutputString((uint8_t*)"Error: SERIAL needs string\n", &USART1_PORT);
        return;
    }

    if(strlen(value) > maxInputLength) {
        SerialOutputString((uint8_t*)"Error: Max 32 chars\n", &USART1_PORT);
    } else {
        SerialOutputString((uint8_t*)"SERIAL: ", &USART1_PORT);
        SerialOutputString((uint8_t*)value, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
}

void sortingOutInput(char buffers[][BUFFER], uint8_t bufIndex) {
    char* input = buffers[bufIndex];

    // Trim whitespace and newlines
    size_t len = strlen(input);
    while(len > 0 && isspace(input[len-1])) {
        input[--len] = '\0';
    }

    // Skip leading whitespace
    char* cmdStart = input;
    while(isspace(*cmdStart)) {
        cmdStart++;
    }

    // Extract command and value
    char* command = cmdStart;
    char* value = strchr(cmdStart, ' ');
    if(value != NULL) {
        *value++ = '\0';
        while(isspace(*value)) value++;
    }

    // Debug print
	SerialOutputString((uint8_t*)"\nProcessing: [", &USART1_PORT);
	SerialOutputString((uint8_t*)cmdStart, &USART1_PORT);
	SerialOutputString((uint8_t*)"]\n", &USART1_PORT);

    // Command processing
    if(strcasecmp(command, "led") == 0) {
        handleNumericCommand("LED", value);
    }
    else if(strcasecmp(command, "timer") == 0) {
        handleNumericCommand("TIMER", value);
    	timerdemo(value);
    }
    else if(strcasecmp(command, "oneshot") == 0) {
    	handleNumericCommand("ONESHOT", value);
    	timer_one_shot(value, timerCallback);
    }
    else if(strcasecmp(command, "serial") == 0) {
        handleSerial(value);
    }
    else {
        SerialOutputString((uint8_t*)"Unknown command: ", &USART1_PORT);
        SerialOutputString((uint8_t*)command, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
    SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    memset(buffers[bufIndex], 0, BUFFER);
}
