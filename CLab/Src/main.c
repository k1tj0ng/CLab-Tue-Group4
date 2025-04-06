/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"
#include "stm32f303xc.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


void finished_transmission(uint32_t bytes_sent) {
	// This function will be called after a transmission is complete

	volatile uint32_t test = 0;
	// make a very simple delay
	for (volatile uint32_t i = 0; i < 0x8ffff; i++) {
		// waste time !
	}
}

void OnRxComplete(char* str, uint32_t len) {
    printf("Received: %.*s\n", len, str);
}

int main(void) {
//	uint8_t *string_to_send = "This is a string !\r\n";

	//void (*completion_function)(uint32_t) = &finished_transmission;

	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);
	Serial_SetRxCallback(OnRxComplete);

	/* Loop forever */
	for(;;) {
		Serial_PollRx(&USART1_PORT);
		char input[32];
		const char terminatingChar = '@';

		SerialInputString(input, terminatingChar, sizeof(input), &USART1_PORT);
		SerialOutputString((uint8_t *)input, &USART1_PORT);
	}
}
