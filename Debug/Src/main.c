#include <stdint.h>
#include <stddef.h>  // Required for NULL definition
#include "stm32f303xc.h"
#include "serial.h"
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

void finished_transmission(uint32_t bytes_sent) {
	// This function will be called after a transmission is complete

	volatile uint32_t test = 0;
	// make a very simple delay
	for (volatile uint32_t i = 0; i < 0x8ffff; i++) {
		// waste time !
	}
}

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

void UART_SendChar(uint8_t data, SerialPort *serial_port) {
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
		}

		serial_port->UART->TDR = data;
}

void UART_SendString(uint8_t *pt, SerialPort *serial_port) {
	uint32_t counter = 0;
		while(*pt) {
			SerialOutputChar(*pt, serial_port);
			counter++;
			pt++;
		}

		serial_port->completion_function(counter);
}

// Example callback
void OnDataReceived(char* str, uint32_t len) {
    UART_SendString("Received: ");
    UART_SendString(str);
    UART_SendString("\r\n");
}

int main(void) {
	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);
    UART_SetRxCallback(OnDataReceived, '#');

    uint8_t *initialiseMessage = "System Ready. Send messages ending with #\r\n";
    UART_SendString(initialiseMessage, &USART1_PORT);

    while (1) {
        // Main application
    }
}
