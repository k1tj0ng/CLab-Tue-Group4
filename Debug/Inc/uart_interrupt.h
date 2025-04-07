#ifndef UART_INTERRUPT_H
#define UART_INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f303xc.h"

#define UART_BUFFER_SIZE 128

void UART_InterruptInit(USART_TypeDef *UART, char terminating_char);
bool UART_DataAvailable(void);
void UART_GetReceivedData(char *dest_buffer, uint32_t max_length);

#endif // UART_INTERRUPT_H
