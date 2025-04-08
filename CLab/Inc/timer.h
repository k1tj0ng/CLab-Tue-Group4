#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Define the callback function using the function pointers
typedef void (*CallbackFunction)(void);

// Timer software module that can trigger the callback function after certain interval
void timer_init(uint32_t interval, CallbackFunction callback);

// Timer interrupt handler
void TIM2_IRQHandler(void);

// Reset the timer with the new interval
void timer_reset(uint32_t interval);

#endif
