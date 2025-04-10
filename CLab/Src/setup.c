#include "stm32f303xc.h"
#include <stddef.h>

// Enable the clocks for GPIOE and TIM2
void enable_clocks() {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;  // Enable the GPIOE clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable the TIM2 clock
}

// Initialise the discovery board I/O
void initialise_board() {
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;  // Set PE8-PE15 as output
}
