#include "handler.h"

#include "stm32f303xc.h"

#include "serial.h"
#include "timer.h"
#include "digital_io.h"

#include <string.h>
#include <stdio.h>


// Global variables (volatile for ISR safety)
volatile uint16_t writePos = 0;       // Current write position
volatile bool bufferReady = false;     // Data ready flag

void USART1_EXTI25_IRQHandler(void) {
    // Check for errors first (clears them automatically on read)
    if (USART1->ISR & (USART_ISR_FE | USART_ISR_ORE)) {
        USART1->ICR |= USART_ICR_FECF | USART_ICR_ORECF; // Clear flags
        return;
    }

    // Only handle RX not empty interrupt
    if (USART1->ISR & USART_ISR_RXNE) {
        uint8_t data = (uint8_t)(USART1->RDR & 0xFF);

        // Handle buffer overflow
        if (writePos >= BUFFER - 1) {
            writePos = 0;
            memset((void*)strings[activeIndex], 0, BUFFER);
            return;
        }

        // Echo back (optional)
        SerialOutputChar(data, &USART1_PORT);

        if (data == '\n' || data == '\r') {
            // Terminate string and mark buffer ready
            strings[activeIndex][writePos] = '\0';
            bufferReady = 1;

            // Swap buffers
            readyIndex = activeIndex;
            activeIndex ^= 1;
            writePos = 0;

            // Clear new active buffer
            memset((void*)strings[activeIndex], 0, BUFFER);
        } else if (data == TERMINATING_CHAR) {

        	SerialOutputString((uint8_t *)"\nTERMINATING CHARACTER DETECTED!", &USART1_PORT);

			SerialOutputString((uint8_t*)"\nThe length is: ", &USART1_PORT);

			char strLenBuffer[10];
			sprintf(strLenBuffer, "%d", writePos);

			SerialOutputString((uint8_t*)strLenBuffer, &USART1_PORT);
			SerialOutputString((uint8_t*)"\n", &USART1_PORT);
        } else {
            // Store regular character
            strings[activeIndex][writePos++] = data;
        }
    }
}

// Timer interrupt handler for TIM2
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // Check if the interrupt flag is raised
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the interrupt flag
        timerCallback();  // Execute the callback function
    }
}

void TIM2_IRQHandler_chaseled() {
    if ((TIM2->SR & TIM_SR_UIF) !=0){
    		chase_led();					// Check if update interrupt
    }
        TIM2->SR &= ~TIM_SR_UIF;            // Clear update interrupt flag
}

// LED interrupt
void EXTI0_IRQHandler(void)
{
	// run the button press handler (make sure it is not null first !)
	if (on_button_press != 0x00) {
		on_button_press();
	}

	// reset the interrupt (so it doesn't keep firing until the next trigger)
	EXTI->PR |= EXTI_PR_PR0;
}
