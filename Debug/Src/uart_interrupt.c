#include "uart_interrupt.h"
#include "stm32f303xc.h"
#include <string.h>

// Receive buffer and control variables
#define RX_BUFFER_SIZE 128
static volatile char rx_buffer[RX_BUFFER_SIZE];
static volatile uint32_t rx_index = 0;
static volatile uint8_t rx_complete = 0;
static volatile char terminator_char = '@';
static SerialPort *current_serial_port = NULL;

// Optional callback function for when message is complete
static void (*message_callback)(char*, uint32_t) = NULL;

void SerialInterruptInit(SerialPort *serial_port, char terminator) {
    // Store the serial port for later use
    current_serial_port = serial_port;

    // Set the terminator character
    terminator_char = terminator;

    // Reset buffer state
    rx_index = 0;
    rx_complete = 0;

    // Enable RXNE interrupt (Receive register not empty)
    serial_port->UART->CR1 |= USART_CR1_RXNEIE;

    // Enable USART interrupt in NVIC
    if (serial_port->UART == USART1) {
        NVIC_SetPriority(USART1_IRQn, 3);
        NVIC_EnableIRQ(USART1_IRQn);
    }
    // Add more UARTs as needed:
    // else if (serial_port->UART == USART2) {
    //     NVIC_SetPriority(USART2_IRQn, 3);
    //     NVIC_EnableIRQ(USART2_IRQn);
    // }
}

void SerialInterruptSetTerminator(char terminator) {
    terminator_char = terminator;
}

void SerialInterruptSetCallback(void (*rx_callback)(char*, uint32_t)) {
    message_callback = rx_callback;
}

uint8_t SerialInterruptCheckMessage(char *buffer, uint32_t buffer_size) {
    if (rx_complete) {
        // Critical section - disable interrupts
        __disable_irq();

        // Copy data from the receive buffer to the output buffer
        uint32_t len = (rx_index < buffer_size - 1) ? rx_index : buffer_size - 1;
        memcpy(buffer, (const char*)rx_buffer, len);
        buffer[len] = '\0'; // Ensure null termination

        // Reset receive buffer
        rx_index = 0;
        rx_complete = 0;

        // Re-enable interrupts
        __enable_irq();

        return 1; // Data was available
    }

    return 0; // No data available
}

// This function should be called from your IRQ handler in main.c
void SerialInterruptHandleRx(uint8_t received_byte) {
    char received_char = (char)received_byte;

    // Store the character in buffer if there's room
    if (rx_index < RX_BUFFER_SIZE - 1) {
        rx_buffer[rx_index++] = received_char;

        // Echo character back to terminal if serial port is available
        if (current_serial_port != NULL) {
            SerialOutputChar(received_char, current_serial_port);
        }

        // Check if terminating character received
        if (received_char == terminator_char) {
            // Null terminate the string
            rx_buffer[rx_index] = '\0';
            rx_complete = 1;

            // Call callback if registered
            if (message_callback != NULL) {
                message_callback((char*)rx_buffer, rx_index);
            }
        }
    } else {
        // Buffer overflow - reset
        rx_index = 0;
    }
}
