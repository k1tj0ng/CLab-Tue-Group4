#ifndef TIMER_HEADER
#define TIMER_HEADRE

#include <stdint.h>


// Enable the clock
void enable_clocks();

// Initialize the timer with a period (ms) and a callback function pointers
void timer_init(uint32_t period_ms, void (*callback_function)(void));

// Reset the timer
void timer_reset(uint32_t period_ms);

// Start the timer
void timer_start(void);

// Stop the timer
void timer_stop(void);

// One-shot timer event with a delay
void timer_one_shot(uint32_t delay_ms, void (*callback_function)(void));

#endif
