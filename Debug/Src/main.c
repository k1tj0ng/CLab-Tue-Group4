#include <stdint.h>
#include <stddef.h>  // Required for NULL definition
#include "stm32f303xc.h"
#include "serial.h"
#include "uart_interrupt.h"
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

// USART1 interrupt handler
void USART1_IRQHandler(void) {
    // Check if receive data register not empty flag is set
    if (USART1->ISR & USART_ISR_RXNE) {
        // Read the received character and pass to serial interrupt handler
        uint8_t received_char = USART1->RDR;
        SerialInterruptHandleRx(received_char);
    }

    // Add other UART interrupt handling if needed (e.g., TX complete)
}


int main(void) {
	  SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

	    // Initialize interrupt-based reception with '@' as terminator
	    SerialInterruptInit(&USART1_PORT, '@');

	    // Optional: Set a callback for message processing
	    SerialInterruptSetCallback(message_received);

	    // Send welcome message
	    SerialOutputString((uint8_t*)"Serial system ready. Type a message ending with @\r\n", &USART1_PORT);

	    char input[32];

	    while (1) {
	        // Method 1: Use polling to check for messages (for simple applications)
	        if (SerialInterruptCheckMessage(input, sizeof(input))) {
	            // Message received, echo it back
	            SerialOutputString((uint8_t*)"\r\nPolling: You sent: ", &USART1_PORT);
	            SerialOutputString((uint8_t*)input, &USART1_PORT);
	            SerialOutputString((uint8_t*)"\r\n", &USART1_PORT);
	        }

	        // Method 2: Use callbacks for message processing (set above)
	        // This happens automatically when a message is received

	        // Your main loop can do other work here
	    }
	}
