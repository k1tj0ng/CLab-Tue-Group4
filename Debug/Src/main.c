#include <stdint.h>
#include <stdio.h>
#include <serial.h>

#include "stm32f303xc.h"
#include "serial.h"
#include "handler.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define ALTFUNCTION 0xA00
#define RXTX 0x770000
#define HIGHSPEED 0xF00
#define BAUDRATE 0x46
#define LED_OUTPUT 0x5x555

void enableInterrupts();


int main(void)
{
	SerialInitialise(BAUD_115200, &USART1_PORT, 0x00);
	enableInterrupts();

	for(;;){}
}

void enableInterrupts()
{
	__disable_irq();

	// Generate an interrupt upon receiving data
	USART1->CR1 |= USART_CR1_RXNEIE_Msk;

	// Set priority and enable interrupts
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	__enable_irq();
}
