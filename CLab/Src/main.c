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

#include "stm32f303xc.h"

#include "serial.h"
#include "timer.h"
#include "digital_io.h"
#include "setup.h"
#include "integration.h"

#include "handler.h"
#include "interrupts.h"

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

int main(void)
{
//	enable_clocks();
//	initialise_board();

	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);
	UARTenableInterrupts();

	/* Loop forever */
	for(;;) {
		if (bufferReady) {
		        sortingOutInput((char (*)[BUFFER])strings, readyIndex);
		        bufferReady = 0;
//		SerialOutputString(string_to_send, &USART1_PORT);
	}
}
}
