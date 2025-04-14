#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

// Define the callback function using the function pointers
typedef void (*CallbackFunction)(void);
extern CallbackFunction timerCallback;
extern volatile bool isOneShot;

// Timer interrupt handler
void TIM2_IRQHandler(void);

// Timer module that can trigger the callback function after a specific interval
void timer_init(uint32_t interval, CallbackFunction callback);

// Timer module that can reset the timer with a new time interval
void timer_reset(uint32_t interval);

// Timer module that can trigger a one-shot event
void timer_one_shot(uint32_t delay, CallbackFunction callback);

// Function to swap the state of the LEDs
void blink_leds(void);

// Function for assessment day
int timerdemo(int interval);

// Timer integration
void timerhandle(int interval);

#endif
