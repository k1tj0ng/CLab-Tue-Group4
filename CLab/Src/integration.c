#include "integration.h"

#include "stm32f303xc.h"
#include "serial.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void sortingOutInput(char buffers[][BUFFER], uint8_t bufIndex) {
    char* input = buffers[bufIndex];

    // 1. Trim whitespace and newlines
    size_t len = strlen(input);
    while(len > 0 && isspace(input[len-1])) {
        input[--len] = '\0';
    }

    // 2. Skip leading whitespace
    char* cmdStart = input;
    while(isspace(*cmdStart)) {
        cmdStart++;
    }

    // 3. Debug print the received input
    SerialOutputString((uint8_t*)"\nProcessing: [", &USART1_PORT);
    SerialOutputString((uint8_t*)cmdStart, &USART1_PORT);
    SerialOutputString((uint8_t*)"]\n", &USART1_PORT);

    // 4. Extract command and value
    char* command = cmdStart;
    char* value = NULL;

    // Find the first space to separate command/value
    char* space = strchr(cmdStart, ' ');
    if(space != NULL) {
        *space = '\0'; // Terminate command
        value = space + 1;

        // Skip spaces after the delimiter
        while(isspace(*value)) {
            value++;
        }
    }

    // 5. Process commands (case-insensitive)
    if(strcasecmp(command, "led") == 0) {
        if(value != NULL) {
            SerialOutputString((uint8_t*)"LED: ", &USART1_PORT);
            SerialOutputString((uint8_t*)value, &USART1_PORT);
            SerialOutputString((uint8_t*)"\n", &USART1_PORT);

            // Here you would call your LED control function
            // e.g., set_led_state(atoi(value));
        } else {
            SerialOutputString((uint8_t*)"Error: LED needs value\n", &USART1_PORT);
        }
    }
    else if(strcasecmp(command, "serial") == 0) {
        SerialOutputString((uint8_t*)"SERIAL command\n", &USART1_PORT);
        // Call serial-specific functions here
    }
    else if(strcasecmp(command, "timer") == 0) {
        if(value != NULL) {
            SerialOutputString((uint8_t*)"TIMER: ", &USART1_PORT);
            SerialOutputString((uint8_t*)value, &USART1_PORT);
            SerialOutputString((uint8_t*)"\n", &USART1_PORT);
        } else {
            SerialOutputString((uint8_t*)"Error: TIMER needs value\n", &USART1_PORT);
        }
    }
    else {
        SerialOutputString((uint8_t*)"Unknown command: ", &USART1_PORT);
        SerialOutputString((uint8_t*)command, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }

    // 6. Clear the processed buffer
    memset(buffers[bufIndex], 0, BUFFER);
}
