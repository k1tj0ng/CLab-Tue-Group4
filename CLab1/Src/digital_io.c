#include "stm32f303xc.h"
#include <stdint.h>
#include "setup.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void (*on_button_press)() = 0x00; // button press function pointer
extern uint16_t led_state = 0x00;

void button_press(void(*callback)(void)){
	on_button_press= callback;
}

void EXTI0_IRQHandler(void)
{
	// run the button press handler (make sure it is not null first !)
	if (on_button_press != 0x00) {
		on_button_press();
	}

	// reset the interrupt (so it doesn't keep firing until the next trigger)
	EXTI->PR |= EXTI_PR_PR0;
}

void enable_interrupt() {
	// Disable the interrupts while messing around with the settings
	//  otherwise can lead to strange behaviour
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts details on large manual page 294)
	// PA0 is on interrupt EXTI0 large manual - page 250
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

void TIM2_IRQHandler_chaseled() {
    if ((TIM2->SR & TIM_SR_UIF) !=0){
    		chase_led();					// Check if update interrupt
    }
        TIM2->SR &= ~TIM_SR_UIF;            // Clear update interrupt flag
}

// function that sets the led state
void set_led_state(uint16_t state) {
	led_state = state;
	GPIOE->ODR = (GPIOE->ODR & 0x00FF) | (state << 8); // set led state
}

// function that gets the led state
void get_led_state() {
	return (GPIOE->ODR >> 8) & 0xFF00;
}
