#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "setup.h"
#include "digital_io.h"
#include "stm32f303xc.h"
#include "led_operator.h"


void turn_on_led(uint8_t BITMASK) {

	 // Clear upper byte, then set it with shifted bitmask
	 GPIOE->ODR = (GPIOE->ODR & 0x00FF) | (BITMASK << 8);

}
