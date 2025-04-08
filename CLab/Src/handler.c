#include "handler.h"
#include "stm32f303xc.h"
#include <serial.h>

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

			SerialOutputString((uint8_t*)"\nThe length is: ", &USART1_PORT);

			char strLenBuffer[10];
			sprintf(strLenBuffer, "%d", str_len);
			SerialOutputString((uint8_t*)strLenBuffer, &USART1_PORT);
		}
	}
}
