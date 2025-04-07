#include "uart_interrupt.h"

static volatile char uart_buffer[UART_BUFFER_SIZE];
static volatile uint32_t buffer_index = 0;
static volatile bool data_ready = false;
static char term_char;

void UART_InterruptInit(USART_TypeDef *UART, char terminating_char) {
    term_char = terminating_char;

    // Enable USART RXNE interrupt
    UART->CR1 |= USART_CR1_RXNEIE;

    // Enable USART global interrupt in NVIC (USART1_IRQn for USART1)
    NVIC_EnableIRQ(USART1_IRQn);  // Adjust this for other UARTs if needed
}

// Interrupt handler for USART1
void USART1_IRQHandler(void) {
    if ((USART1->ISR & USART_ISR_RXNE) && !(USART1->CR1 & USART_CR1_OVER8)) {
        char received = (char)(USART1->RDR & 0xFF);

        if (!data_ready && buffer_index < UART_BUFFER_SIZE - 1) {
            uart_buffer[buffer_index++] = received;

            if (received == term_char) {
                uart_buffer[buffer_index] = '\0';
                data_ready = true;
            }
        }
    }
}

bool UART_DataAvailable(void) {
    return data_ready;
}

void UART_GetReceivedData(char *dest_buffer, uint32_t max_length) {
    if (data_ready) {
        for (uint32_t i = 0; i < buffer_index && i < max_length - 1; i++) {
            dest_buffer[i] = uart_buffer[i];
        }
        dest_buffer[buffer_index] = '\0';
        buffer_index = 0;
        data_ready = false;
    }
}
