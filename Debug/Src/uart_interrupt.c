#include "uart_interrupt.h"
#include "serial.h"
#include "stm32f303xc.h"
#include <string.h>

// Receive buffer and control variables
#define RX_BUFFER_SIZE 128
static volatile char rx_buffer[RX_BUFFER_SIZE];  // Receive buffer
static volatile uint32_t rx_index = 0;  // Index to track where to store the next byte
static volatile uint8_t rx_complete = 0;  // Flag to indicate if data reception is complete
static SerialPort *current_serial_port = NULL;  // Pointer to the current serial port

// Optional callback function for when data is received
static void (*data_received_callback)(char*, uint32_t) = NULL;

void SerialInterruptInit(SerialPort *serial_port) {
    // Store the serial port for later use
    current_serial_port = serial_port;

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

void SerialInterruptSetCallback(void (*rx_callback)(char*, uint32_t)) {
    data_received_callback = rx_callback;
}

// This function should be called from your IRQ handler in main.c
void SerialInterruptHandleRx(uint8_t received_byte) {
    // Store the character in buffer if there's room
    if (rx_index < RX_BUFFER_SIZE - 1) {
        rx_buffer[rx_index++] = (char)received_byte;  // Store the byte in the buffer

        // Optionally echo character back to terminal if serial port is available
        if (current_serial_port != NULL) {
            SerialOutputChar((char)received_byte, current_serial_port);
        }

        // Call the callback function if registered (for continuous data processing)
        if (data_received_callback != NULL) {
            data_received_callback((char*)rx_buffer, rx_index);  // Call callback with the current buffer
        }
    } else {
        // Buffer overflow - reset
        rx_index = 0;
    }
}

uint8_t SerialInterruptCheckMessage(char *buffer, uint32_t buffer_size) {
    // This function may not be necessary anymore since you're processing data continuously
    return 0;
}

// USART1 interrupt handler
void USART1_IRQHandler(void) {
    // Check if receive data register not empty flag is set
    if (USART1->ISR & USART_ISR_RXNE) {
        // Read the received character and process it
        uint8_t received_char = USART1->RDR;
        SerialInterruptHandleRx(received_char);  // Handle received byte
    }
}

