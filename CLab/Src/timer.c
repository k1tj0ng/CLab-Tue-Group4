#include "stm32f303xc.h"
#include "timer.h"
#include <stddef.h>

// Define the variables
void (*periodic_callback_func)(void) = NULL;
void (*one_shot_callback_func)(void) = NULL;
volatile uint32_t one_shot_delay = 0;
volatile uint32_t one_shot_counter = 0;
volatile uint32_t current_period = 0;

// Enable the clocks for GPIOE and TIM2
void enable_clocks() {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;  // Enable the GPIOE clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable the TIM2 clock
}

// Initialize the timer with period (ms) and a callback function
void timer_init(uint32_t period_ms, void (*callback_function)(void)) {
    periodic_callback_func = callback_function;
    current_period = period_ms;

    // Configure prescaler for 1 ms time base
    TIM2->PSC = 7999;			// (8 MHz / 1000 ms) - 1
    TIM2->ARR = period_ms - 1;	// Set period (ARR = period - 1)
    TIM2->CNT = 0;				// Reset the counter
    TIM2->CR1 |= TIM_CR1_CEN;	// Start the timer
    NVIC_EnableIRQ(TIM2_IRQn);  // Enable the timer interrupt
}

// Reset the timer with a new period
void timer_reset(uint32_t period_ms) {
	current_period = period_ms;		// Update the period
    TIM2->ARR = period_ms - 1;	// Reset the period
    TIM2->CNT = 0;				// Reset the counter
}

// Start the timer
void timer_start(void) {
    TIM2->CR1 |= TIM_CR1_CEN;
}

// Stop the timer
void timer_stop(void) {
    TIM2->CR1 &= ~TIM_CR1_CEN;
}

// One-shot timer event with a delay
void timer_one_shot(uint32_t delay_ms, void (*callback_function)(void)) {
    one_shot_delay = delay_ms;
    one_shot_counter = 0;
    one_shot_callback_func = callback_function;

    TIM2->ARR = 1;				// Set the timer period to 1 tick
    TIM2->CNT = 0;				// Reset the counter
    TIM2->CR1 |= TIM_CR1_CEN;	// Start the timer
    NVIC_EnableIRQ(TIM2_IRQn);	// Enable the timer interrupt
}

// Timer interrupt handler
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {	// Check if update interrupt flag is set
        TIM2->SR &= ~TIM_SR_UIF;	// Clear the interrupt flag

        // Debugging: Toggle LED to check if interrupt is triggered
        GPIOE->ODR ^= 0x1;  // Toggle an LED (PE8 for example) to see if interrupt is firing

        // Handle periodic callback if the timer overflow
        if (periodic_callback_func) {
            GPIOE->ODR ^= 0x1;
            periodic_callback_func();
        }

        // Handle one-shot callback after the specified delay
        if (one_shot_callback_func) {
            one_shot_counter++;
            if (one_shot_counter >= one_shot_delay) {
                one_shot_callback_func();
                TIM2->CR1 &= ~TIM_CR1_CEN;  // Stop the timer after the one-shot event
            }
        }
    }
}

