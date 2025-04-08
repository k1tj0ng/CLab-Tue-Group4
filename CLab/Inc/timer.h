#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Define the callback function using the function pointers
typedef void (*CallbackFunction)(void);

// Timer module that can trigger the callback function after a specific interval
void timer_init(uint32_t interval, CallbackFunction callback);

// Timer interrupt handler
void TIM2_IRQHandler(void);

// Reset the timer with the current interval
void timer_reset(uint32_t interval);

// Timer module that can trigger one-shot event
void timer_one_shot(uint32_t delay, CallbackFunction callback);

#endif
