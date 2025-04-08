#include "interrupts.h"
#include "stm32f303xc.h"

void UARTenableInterrupts()
{
	__disable_irq();

	// Generate an interrupt upon receiving data
	USART1->CR1 |= USART_CR1_RXNEIE_Msk;

	// Set priority and enable interrupts
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	__enable_irq();
}
