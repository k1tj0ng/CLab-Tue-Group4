#include <stdint.h>
#include <stddef.h>  // Required for NULL definition
#include "stm32f303xc.h"
#include "serial.h"
#include "system_stm32f3xx.h"

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

int main(void) {
	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

    while (1) {
    	char input[32];
		const char terminatingChar = '@';

		SerialInputString(input, terminatingChar, sizeof(input), &USART1_PORT);
		SerialOutputString((uint8_t *)input, &USART1_PORT);
    }
}
