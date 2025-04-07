#include "stm32f303xc.h"
#include "timer.h"
#include <stddef.h>
#include "setup.h"

//Define the callback function
static CallbackFunction timerCallback = NULL;

// Timer interrupt handler for TIM2
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // Check if the interrupt flag is raised
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the interrupt flag
        timerCallback();  // Execute the callback function
    }
}

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
