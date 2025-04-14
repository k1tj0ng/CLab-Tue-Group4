#include "timer.h"

#include "gpioe_config.h"
#include "stm32f303xc.h"
#include "handler.h"

#include <stddef.h>

//Define the callback function
CallbackFunction timerCallback = NULL;

// Flag to check the mode of the timer (oneshot or periodic)
volatile bool isOneShot = false;

// Timer that triggers the callback (blink_led) every (interval) millisecond
void timer_init(uint32_t interval, CallbackFunction callback) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop the timer
	isOneShot = false;		// Flag for periodic timer mode

	// Set up the timer prescaler, for 1khZ frequency = (8MHz:1000)-1
	TIM2->PSC = 7999;
	TIM2->ARR = interval;		// Set the time interval
	TIM2->CNT = 0;			// Reset the counter

	TIM2->DIER |= TIM_DIER_UIE;	// Enable interrupt when there is overflow
	NVIC_EnableIRQ(TIM2_IRQn);	// Enable interrupt in the NVIC
	
	timerCallback = callback;	// Set the callback function
	
	TIM2->CR1 |= TIM_CR1_CEN;	// Start the timer
}

// Timer module to reset the timer with a new interval
void timer_reset(uint32_t new_interval) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop timer
	TIM2->CNT = 0;			// Reset counter
	TIM2->ARR = new_interval;	// Update ARR
	TIM2->EGR |= TIM_EGR_UG;	// Force update event to load ARR
	TIM2->CR1 |= TIM_CR1_CEN;	// Restart timer
}

// Timer that triggers a one-shot event after (delay) millisecond
void timer_one_shot(uint32_t delay, CallbackFunction callback) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop timer
	isOneShot = true;		// Flag for oneshot timer mode

	// Set up the timer prescaler
	TIM2->PSC = 7999;
	TIM2->ARR = delay;
	TIM2->CNT = 0;

	// Enable interrupt
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);

	timerCallback = callback;

	TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
}

// Example for the callback function
// This function will be called if there is a timer overflow
void blink_leds(void) {
	static uint8_t led_state = 0;
	GPIOE->ODR = led_state ? 0xFF00 : 0x0000;	// Toggle LEDs on PE8-PE15
	led_state = !led_state;  			// Flip the state
}


// For the timer demonstration
int timerdemo(int interval) {
//int main (int interval) {
	enable_clocks();
	initialise_board();

	// Timer that triggers the callback (blink LED) every x millisecond
	timer_init(1000, blink_leds); // Input the time interval in millisecond

	// Delay for a few seconds or do something
	for (volatile int i = 0; i < 5000000; i++) {}

	timer_reset(100);	// Change the interval

	// Delay for a few seconds or do something
	for (volatile int i = 0; i < 5000000; i++) {}

	timer_one_shot(3000, blink_leds);
}

void timerhandle(int interval) {
    static bool timerRunning = false;

    if (!timerRunning) {
        timer_init(interval, blink_leds);
        timerRunning = true;
    } else {
        timer_reset(interval);
    }
}
