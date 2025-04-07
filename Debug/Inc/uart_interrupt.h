#ifndef UART_INTERRUPT_H
#define UART_INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>
#include <serial.h>
#include "stm32f303xc.h"

void SerialInterruptInit(SerialPort *serial_port, char terminator);

uint8_t SerialInterruptCheckMessage(char *buffer, uint32_t buffer_size);

void SerialInterruptSetTerminator(char terminator);

// Register a callback function to be called when a complete message is received
void SerialInterruptSetCallback(void (*rx_callback)(char*, uint32_t));

#endif /* SERIAL_INTERRUPT */
