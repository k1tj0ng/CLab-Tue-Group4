#include <stdint.h>
#include <stddef.h>  // Required for NULL definition
#include "stm32f303xc.h"
#include "system_stm32f3xx.h"

// Callback function pointer (initialized to NULL)
void (*rx_complete_callback)(char* str, uint32_t len) = NULL;

// Double buffer implementation
#define RX_BUFFER_SIZE 128
static volatile char rx_buffers[2][RX_BUFFER_SIZE];
static volatile uint8_t active_buffer = 0;
static volatile uint32_t rx_position = 0;
static volatile char terminator = '#';

#define SystemCoreClock 72000000

void USART1_IRQHandler(void) {
    // RX Handling
    if (USART1->ISR & USART_ISR_RXNE) {  // If RXNE flag is set (data received)
        char received_char = USART1->RDR;  // Read received character

        // Store in active buffer
        if (rx_position < RX_BUFFER_SIZE - 1) {
            rx_buffers[active_buffer][rx_position++] = received_char;

            // Check for termination
            if (received_char == terminator || rx_position >= RX_BUFFER_SIZE - 1) {
                rx_buffers[active_buffer][rx_position] = '\0';  // Null terminate the string

                // Call callback function if set
                if (rx_complete_callback != NULL) {
                    rx_complete_callback((char*)rx_buffers[active_buffer], rx_position);
                }

                active_buffer ^= 1;  // Toggle between buffer 0 and 1
                rx_position = 0;     // Reset position for new data
            }
        }
    }
}

void UART_Init(uint32_t baud_rate) {
    // 1. Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // 2. Configure GPIO (PA9 = TX, PA10 = RX)
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (2 << GPIO_MODER_MODER9_Pos) | (2 << GPIO_MODER_MODER10_Pos);
    GPIOA->AFR[1] |= (7 << (4 * 1)) | (7 << (4 * 2)); // AF7

    // 3. Configure USART
    USART1->BRR = SystemCoreClock / baud_rate;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;

    // 4. Enable interrupt
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
}

void UART_SetRxCallback(void (*callback)(char*, uint32_t), char term_char) {
    rx_complete_callback = callback;
    terminator = term_char;
    active_buffer = 0;
    rx_position = 0;
}

void UART_SendChar(char c) {
    while (!(USART1->ISR & USART_ISR_TXE));  // Wait for TXE (transmit buffer empty)
    USART1->TDR = c;  // Send character
}

void UART_SendString(const char* str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

// Example callback
void OnDataReceived(char* str, uint32_t len) {
    UART_SendString("Received: ");
    UART_SendString(str);
    UART_SendString("\r\n");
}

int main(void) {
    UART_Init(115200);
    UART_SetRxCallback(OnDataReceived, '#');

    UART_SendString("System Ready. Send messages ending with #\r\n");

    while (1) {
        // Main application
    }
}
