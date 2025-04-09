#include "stm32f303xc.h"
#include "timer.h"
#include <stddef.h>
#include "setup.h"

//Define the callback function and isOneShot boolen
static CallbackFunction timerCallback = NULL;
volatile bool isOneShot = false;

// Timer interrupt handler for TIM2
void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {  // Check if the interrupt flag is raised
		TIM2->SR &= ~TIM_SR_UIF;  // Clear the interrupt flag
		
		// Execute the callback function
		timerCallback();
		
		// Check if one-shot behavior is requested
		if (isOneShot) {
			GPIOE->ODR |= (1<<8); // Turn on the LED
			// Disable the timer to stop it after the callback
			TIM2->CR1 &= ~TIM_CR1_CEN;
			isOneShot = false; // Set back the isOneShot to false
			NVIC_DisableIRQ(TIM2_IRQn); //Disable the interrupt for timer 2
		}
	}
}

// Timer that triggers the callback (blink LED) every x millisecond
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

// One-shot timer function
void timer_one_shot(uint32_t delay, CallbackFunction callback) {
	// Set the delay and callback function
	timerCallback = callback;
	isOneShot = true;
	
	TIM2->CNT = 0;      // Reset the timer counter
	TIM2->ARR = delay;  // Reload the ARR register with the new interval
	TIM2->CR1 |= TIM_CR1_CEN;
	
	// Re-initialize the timer
	timer_init(delay, timerCallback);
}
