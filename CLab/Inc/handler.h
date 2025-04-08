#ifndef HANDLER_H
#define HANDLER_H

#include <stdbool.h>
#include <stdint.h>

void USART1_EXTI25_IRQHandler();

extern volatile uint16_t writePos;     // Current write position
extern volatile bool bufferReady;

#endif
