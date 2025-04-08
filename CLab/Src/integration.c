#include "integration.h"

#include "stm32f303xc.h"
#include "serial.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void sortingOutInput(char strings[][BUFFER]) {
	SerialOutputString((uint8_t*)"\nDEBUG - Raw Input: [", &USART1_PORT);
	SerialOutputString((uint8_t*)strings[0], &USART1_PORT);
	SerialOutputString((uint8_t*)"]\n", &USART1_PORT);
    char *command;
    char *value;
    char *followUp = strings[0];

    strings[0][strcspn(strings[0], "\n")] = '\0';

    command = strtok_r(followUp, " ", &followUp);
    if (command == NULL) {
        SerialOutputString((uint8_t*)"Error: No command\n", &USART1_PORT);
        return;
    }

    value = strtok_r(NULL, " ", &followUp);

    if (strcmp(command, "led") == 0) {
        SerialOutputString((uint8_t*)"LED command\n", &USART1_PORT);
//        if (value) {
//            SerialOutputString((uint8_t*)value, &USART1_PORT);
//            // Here you would call your LED control function with the value
//            // e.g., led_control(value);
//        }
    }
    else if (strcmp(command, "serial") == 0) {
        SerialOutputString((uint8_t*)"SERIAL command\n", &USART1_PORT);
        // Call serial-specific function
    }
    else if (strcmp(command, "oneshot") == 0) {
        SerialOutputString((uint8_t*)"ONESHOT command\n", &USART1_PORT);
//        if (value) {
//            SerialOutputString((uint8_t*)value, &USART1_PORT);
//            // Call oneshot function with value
//        }
//        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
    else if (strcmp(command, "timer") == 0) {
        SerialOutputString((uint8_t*)"TIMER command\n", &USART1_PORT);
//        if (value) {
//            SerialOutputString((uint8_t*)value, &USART1_PORT);
//            // Call timer function with value
//        }
//        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
    else {
        SerialOutputString((uint8_t*)"Unknown command: ", &USART1_PORT);
        SerialOutputString((uint8_t*)command, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
}
