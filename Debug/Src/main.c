#include <stdint.h>
#include <stdio.h>
#include <serial.h>
#include "stm32f303xc.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define ALTFUNCTION 0xA00
#define RXTX 0x770000
#define HIGHSPEED 0xF00
#define BAUDRATE 0x46
#define BUFFER 32
#define LED_OUTPUT 0x5555

void enableUSART1();
void enableLEDs();
void enableInterrupts();
void USART1_IRQHandler();

// Buffer to store incoming characters
unsigned char string[BUFFER];
unsigned char TERMINATING_CHAR = '#';
int i = 0;
int str_len = 0;

int main(void)
{
	SerialInitialise(BAUD_115200, &USART1_PORT, 0x00);
	enableLEDs();
	enableInterrupts();

//	char terminatingChar = '#';

//	SerialInputString(string, terminatingChar, sizeof(string), &USART1_PORT);

	for(;;)
	{}
}

void enableLEDs()
{
	// Enable clock for Port E (LEDs)
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

	// Get the most significant 16 bits of port mode register as that is where the mode for the LEDs are defined
	uint16_t* portMode = ((uint16_t*)&(GPIOE->MODER))+1;

	// Set the mode of the port pins to output since they are LEDs
	*portMode = LED_OUTPUT;
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

void USART1_EXTI25_IRQHandler()
{
	// Check for overrun or frame errors
	if ((USART1->ISR & USART_ISR_FE_Msk) && (USART1->ISR & USART_ISR_ORE_Msk))
	{
		return;
	}

	// If we have stored the maximum amount, stop
	if (i == BUFFER)
	{
		return;
	}

	if (USART1->ISR & USART_ISR_RXNE_Msk)
	{
		// Read data
//		SerialOutputString((uint8_t*)"Message received: ", &USART1_PORT);
//		SerialOutputString((uint8_t*)string, &USART1_PORT);
//		unsigned char data = (uint8_t) USART1->RDR;

		unsigned char data = (uint8_t) USART1->RDR;
//		SerialOutputString((uint8_t*)"Message received: ", &USART1_PORT);
		SerialOutputChar((uint8_t *)data, &USART1_PORT);
		string[i] = data;
		i++;
		str_len++;
		if (data == TERMINATING_CHAR){
			str_len--;

			SerialOutputString((uint8_t*)"\nThe length is:", &USART1_PORT);

//			char strLenBuffer[10] = IntToStr(str_len)
			SerialOutputString((uint8_t*)str_len, &USART1_PORT);
		}
		//SerialOutputString((uint8_t*)string, &USART1_PORT);



//		// Store the read data
//		string[i] = data;
//		i++;
//
//		// Toggle LEDs
//		uint8_t* lights = ((uint8_t*)&(GPIOE->ODR)) + 1;
//		*lights = !(*lights);
	}
}
