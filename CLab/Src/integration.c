#include "integration.h"

#include "stm32f303xc.h"
#include "serial.h"
#include "timer.h"
#include "digital_io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Numerical command handler
void handleNumericCommand(const char* name, const char* value) {
	// If no error display error message on the terminal
    if(value == NULL){
        char msg[BUFFER];
        sprintf(msg, "Error: %s needs value\n", name);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
        return;
    }

    char* endptr;
    long num = strtol(value, &endptr, 10);
    int digits = snprintf(NULL, 0, "%ld", num);

    // If value is invalid, display error message otherwise continue.
    if(*endptr != '\0' || num < 0 || digits > 8) {
        SerialOutputString((uint8_t*)"Error: Invalid number\n", &USART1_PORT);
    } else {
        char msg[BUFFER];
        sprintf(msg, "%s: %ld\n", name, num);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
    }
}

// String command handler
void handleSerial(const char* value) {
	// If no input, display error message
    if(value == NULL) {
        SerialOutputString((uint8_t*)"Error: SERIAL needs string\n", &USART1_PORT);
        return;
    }

    // If input is too long, display error message otherwise display input and continue.
    if(strlen(value) > BUFFER) {
        SerialOutputString((uint8_t*)"Error: Max 32 chars\n", &USART1_PORT);
    } else {
        SerialOutputString((uint8_t*)"SERIAL: ", &USART1_PORT);
        SerialOutputString((uint8_t*)value, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
}

// Integration function. Sorting output to needed function.
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
        uint8_t bitmask = strtol(value, NULL, 2);
        set_led_state(bitmask);
        //set_led_state((uint16_t)value);
    }
    else if(strcasecmp(command, "timer") == 0) {
    	static bool timerRunning = false;
        handleNumericCommand("TIMER", value);
        uint32_t interval = strtol(value, NULL, 10);

        if (!timerRunning) {
            timer_init(interval, blink_leds);
            timerRunning = true;
        } else {
            timer_reset(interval);
        }
    }
    else if(strcasecmp(command, "oneshot") == 0) {
    	handleNumericCommand("ONESHOT", value);
    	uint32_t delay = strtol(value, NULL, 10);
    	timer_one_shot(delay, blink_leds);
    //	timer_one_shot(value, timerCallback);
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
