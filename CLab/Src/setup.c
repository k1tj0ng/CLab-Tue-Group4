#include "stm32f303xc.h"
#include <stddef.h>

// Enable the clocks for GPIOE and TIM2
void enable_clocks() {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;  // Enable the GPIOE clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable the TIM2 clock
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
