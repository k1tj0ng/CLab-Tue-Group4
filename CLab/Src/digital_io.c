#include "digital_io.h"

#include "gpioe_config.h"
#include "handler.h"
#include "interrupts.h"
#include "timer.h"

#include "stm32f303xc.h"

#include <stdint.h>

void (*on_button_press)() = 0x00; // button press function pointer


// Callback function to be called on button press
void button_press(void(*callback)(void)){
	on_button_press= callback;
}





void chase_led(){
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	uint16_t GetLEDState(void);					//call get function

	*led_register <<= 1;						//left shift pointer to LED
	if (*led_register == 0) {					//if the end is not reached continue
		*led_register = 1;
	}
	void SetLEDState(uint16_t state);			//call set function

}




void enable_interrupt() {
	// Disable the interrupts while messing around with the settings
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts
	// EXTI0 in  SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

	//  Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0; // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module that EXTI0 interrupts should be handled
	NVIC_SetPriority(EXTI0_IRQn, 1);  // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}

//function to enable prescaler
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
	TIM2_IRQHandler();

}



// function that sets the led state
void set_led_state(uint8_t state) {
	TIM2->CR1 &= ~TIM_CR1_CEN;		// Clear timer flag
	GPIOE->ODR &= ~(0xFF << 8);       // Clear bits 8–15 (LEDs)
	GPIOE->ODR |= (state << 8);       // Set new LED state
}

// function that gets the led state
uint8_t get_led_state() {
	return (GPIOE->ODR >> 8) & 0xFF00;
}

