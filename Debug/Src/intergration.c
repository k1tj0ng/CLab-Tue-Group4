#include "serial.h"
#include <stddef.h>

#include "stm32f303xc.h"

void intergrationTask(char *input) {

	char command[16];
	char valueStr[33];

	if (sscanf(input, "%s %s", command, valueStr) != 2) {
		// return wrong command
		return;
	}

	if (strcmp(command, "led") == 0) {
		// run the LED
	}

	if (strcmp(command, "serial") == 0) {

	}

	if (strcmp(command, "oneshot") == 0) {

	}

	if (strcmp(command, "timer") == 0) {

	}
}
