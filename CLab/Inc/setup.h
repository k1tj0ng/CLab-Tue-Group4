#ifndef SETUP_HEADER
#define SETUP_HEADER

#include "stm32f303xc.h"
#include <stddef.h>

void enable_clocks();
void TIM2_IRQHandler(void);

#endif
