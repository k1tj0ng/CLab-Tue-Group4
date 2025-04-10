#include "digital_io.h"

#include "handler.h"
#include "interrupts.h"
#include "timer.h"

#include "setup.h"
#include "stm32f303xc.h"

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void (*on_button_press)() = 0x00; // button press function pointer
extern uint16_t led_state = 0x00;

void button_press(void(*callback)(void)){
	on_button_press= callback;
}


void chase_led(){
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	*led_register <<= 1;
	if (*led_register == 0) {
		*led_register = 1;
	}
}

void enable_prescaler(int delay_value){

	TIM2->ARR = 0x01;
	TIM2->CNT = 0x00;
	asm("NOP");
	asm("NOP");
	asm("NOP");
	TIM2->ARR = 0xffffffff;
	__disable_irq();

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2 clock
	TIM2->PSC = 7999;                       // Set prescaler to 7999
	TIM2->ARR = 1000;                      // Set auto-reload for 1-second interval
	TIM2->DIER |= TIM_DIER_UIE;            // Enable update interrupt
    TIM2->CR1 |= TIM_CR1_CEN;              // Enable Timer 2
	NVIC_EnableIRQ(TIM2_IRQn);             // Enable Timer 2 interrupt in NVIC
	TIM2->ARR = delay_value;
	__enable_irq();
	TIM2_IRQHandler_chaseled();

}

// function that sets the led state
void set_led_state(uint16_t state) {
	led_state = state;
	enable_clocks();
	initialise_board();
	GPIOE->ODR &= ~(0xFF << 8);       // Clear bits 8–15 (LEDs)
	GPIOE->ODR |= (state << 8);       // Set new LED state

//	GPIOE->ODR = (GPIOE->ODR & 0x00FF) | (state << 8); // set led state
}

// function that gets the led state
void get_led_state(void) {
	return (GPIOE->ODR >> 8) & 0xFF00;
}
