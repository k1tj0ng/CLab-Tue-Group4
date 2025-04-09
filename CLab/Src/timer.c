#include "timer.h"

#include "stm32f303xc.h"
#include "setup.h"
#include "handler.h"

#include <stddef.h>

//Define the callback function
CallbackFunction timerCallback = NULL;

// Timer that trigger the callback (blink LED) every x millisecond
void timer_init(uint32_t interval, CallbackFunction callback) {
    // Set up the timer prescaler
    TIM2->PSC = 7999;  // (8MHz:1000)-1
    TIM2->ARR = interval;

	// Enable interrupt when there is overflow
	TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    // Set the callback function
    timerCallback = callback;

    // Start the timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

// Reset the timer with the new interval
void timer_reset(uint32_t interval) {
    TIM2->CNT = 0;         // Reset the timer counter
    TIM2->ARR = interval;  // Reload the ARR register with the new interval
    TIM2->CR1 |= TIM_CR1_CEN;

    // Re-initialize the timer
    timer_init(interval, timerCallback);
}

void blink_leds(void) {
	// This function will be called if there is a timer overflow
	static uint8_t led_state = 0;
	GPIOE->ODR = led_state ? 0xFF00 : 0x0000;  // Toggle LEDs on PE8-PE15
	led_state = !led_state;  // Flip the state
}


int timerdemo(int interval) {
	enable_clocks();
	initialise_board();

	// Timer that triggers the callback (blink LED) every x millisecond
	timer_init((uint32_t)interval, blink_leds); // Input the time interval in millisecond

	// Delay for a few seconds or do something
	for (volatile int i = 0; i < 5000; i++) {
	}

	timer_reset(interval);	// Change the interval

//	while (1) {}
}
