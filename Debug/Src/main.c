#include <stdint.h>
#include <stddef.h>  // Required for NULL definition
#include "stm32f303xc.h"
#include "serial.h"
//#include "uart_interrupt.h"
#include "system_stm32f3xx.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

volatile void (*on_usart1_receive)(char c) = NULL;

//void USART1_IRQHandler() {
//	SerialOutputString((uint8_t*)"Interrupt working yay\r\n", &USART1_PORT);
//    // Check if RXNE flag is set (data received)
//	if ((USART1->ISR & USART_ISR_RXNE) != 0) {
//	        char received_char = USART1->RDR;  // Reading clears RXNE
//
//	        if ((USART1->ISR & (USART_ISR_FE | USART_ISR_ORE)) == 0) {
//	            if (on_usart1_receive != NULL) {
//	                on_usart1_receive(received_char);
//	            }
//	        }
//	    } else {
//    	USART1 -> CR1 = 0;
//    	USART1 -> CR1 |= USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
//    }
//
//    USART1 -> ISR &= ~USART_ISR_RXNE;
//    USART1 -> CR1 |= USART_RQR_RXFRQ;
//}

void USART1_IRQHandler() {
    SerialOutputString((uint8_t*)"Interrupt working yay\r\n", &USART1_PORT);

    // Check if RXNE flag is set (data received)
    if (USART1->ISR & USART_ISR_RXNE) {
            // Read the received data (this clears the RXNE flag)
            uint8_t received_data = (uint8_t)(USART1->RDR & 0xFF);

            // Do something with the received data
            // For example, echo it back:
            USART1->TDR = received_data;

            // Or process it in some way
        }


    // No need to clear RXNE manually or reset anything else
}

void enable_usart1_interrupt(void) {
    __disable_irq();

    SerialOutputString((uint8_t*)"Interrupt is working right now!\r\n", &USART1_PORT);

    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);

    // RXNEIE should already be set in SerialInitialise or CR1 |= later
    USART1->CR1 |= USART_CR1_RXNEIE;

    __enable_irq();
}

void finished_transmission(uint32_t bytes_sent) {
	// This function will be called after a transmission is complete

	volatile uint32_t test = 0;
	// make a very simple delay
	for (volatile uint32_t i = 0; i < 0x8ffff; i++) {
		// waste time !
	}
}

//void (*on_usart1_receive)(char c) = 0x00;

void handle_received_char(char c) {
    // Do something with the character (e.g., echo it back)
    SerialOutputChar(c, &USART1_PORT);

    // Or store it, process it, etc.
}


int main(void) {
    // Initialize serial communication with 115200 baud rate
    SerialInitialise(BAUD_115200, &USART1_PORT, 0x00);

    // Initialize interrupt-based reception without needing to pass the terminator
//    USART1->CR1 |= USART_CR1_RXNEIE;
//    __disable_irq();
//
//    NVIC_EnableIRQ(USART1_IRQn);
//    NVIC_SetPriority(USART1_IRQn, 1);

//    SerialInterruptInit(&USART1_PORT);

//    SerialInterruptSetReceiveCallback(on_usart1_receive);

    // Send welcome message

    on_usart1_receive = handle_received_char;

    SerialOutputString((uint8_t*)"Serial system ready. Type a message ending with @\r\n", &USART1_PORT);

    enable_usart1_interrupt();


    while (1) {}
}
