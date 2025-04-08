#include "handler.h"
#include "stm32f303xc.h"
#include <serial.h>
#include <string.h>


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
        } else {
            // Store regular character
            strings[activeIndex][writePos++] = data;
        }
    }
}
