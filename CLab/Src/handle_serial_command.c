#include <string.h>
#include <ctype.h>
#include <gpioe_config.h>
#include <stdlib.h>
#include <stdio.h>
#include "digital_io.h"
#include "stm32f303xc.h"
#include "handle_serial_command.h"

void handle_serial_command(char* serial_command) {
	char command[16];
	char operand[128];

    if (sscanf(serial_command, "%15s %[^\n]", command, operand) != 2) {
        return 0xFF;
    }

    if (strcmp(command, "led") == 0) {
    	uint8_t bitmask =0;
    	char *ptr = operand;
    	while (*ptr) {
    		bitmask <<= 1;

    		if (*ptr == '1') {
    			bitmask |= 1;
    		}

    		ptr++;
    	}
     return bitmask;
    } else if (strcmp(command, 'serial') == 0) {
    	return operand;
    } else if (strcmp(command, 'oneshot') == 0) {
    	int oneshot_duration = 0;
    	oneshot_duration = atoi(operand);
    	return oneshot_duration;
    } else if (strcmp(command, 'timer') == 0) {
    	int timer_duration = 0;
    	timer_duration = atoi(operand);
    	return timer_duration;
    }
}
