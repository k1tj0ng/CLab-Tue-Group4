// uart_interrupt.h
#ifndef UART_INTERRUPT_H
#define UART_INTERRUPT_H

#include "serial.h"  // Include serial.h if needed for SerialPort definition

// Function to initialize serial interrupts
void SerialInterruptInit(SerialPort *serial_port);

// Other function declarations (if any)

void SerialInterruptSetCallback(void (*rx_callback)(char*, uint32_t));

void SerialInterruptHandleRx(uint8_t received_byte);

uint8_t SerialInterruptCheckMessage(char *buffer, uint32_t buffer_size);

void USART1_IRQHandler(void);

#endif
