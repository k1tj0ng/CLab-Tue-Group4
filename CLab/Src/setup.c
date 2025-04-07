#include "stm32f303xc.h"
#include <stddef.h>

// Enable the clocks for GPIOE and TIM2
void enable_clocks() {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;  // Enable the GPIOE clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable the TIM2 clock
}

