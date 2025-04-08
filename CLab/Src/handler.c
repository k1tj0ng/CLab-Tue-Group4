#include "handler.h"
#include "stm32f303xc.h"
#include <serial.h>

void USART1_EXTI25_IRQHandler() {
	// Check for overrun or frame errors
	if ((USART1->ISR & USART_ISR_FE_Msk) && (USART1->ISR & USART_ISR_ORE_Msk))
	{
		return;
	}

	// If we have stored the maximum amount, stop
	if (i == BUFFER)
	{
		i = 0;
		memset((void*)strings[activeIndex], 0, BUFFER); // Clear corrupted buffer
		return;
	}

	if (USART1->ISR & USART_ISR_RXNE_Msk) {
		unsigned char data = (uint8_t) USART1->RDR;
		str_len++;

		if (data == '\n') {
			SerialOutputString((uint8_t*)"\n", &USART1_PORT);
			sortingOutInput((char (*)[BUFFER])strings);
			readyIndex = activeIndex;
			activeIndex ^= 1;

			 memset((void*)strings[activeIndex], 0, BUFFER); // Clear new active buffer
			i = 0;

			// Buffer indicator checker
//			char bufferIndicator[10];
//			sprintf(bufferIndicator, "%d", activeIndex);
//			SerialOutputString((uint8_t*)bufferIndicator, &USART1_PORT);

//			str_len = 0;
		} else if (data == TERMINATING_CHAR) {
			str_len--;

			SerialOutputString((uint8_t*)"The length is: ", &USART1_PORT);

			char strLenBuffer[10];
			sprintf(strLenBuffer, "%d", str_len);

			SerialOutputString((uint8_t*)strLenBuffer, &USART1_PORT);
			SerialOutputString((uint8_t*)"\n", &USART1_PORT);
		} else {
			SerialOutputChar((uint8_t *)data, &USART1_PORT);
			strings[activeIndex][i++] = data;
			}
		}
	}
