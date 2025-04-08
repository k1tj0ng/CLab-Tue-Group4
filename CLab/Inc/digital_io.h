// digital_io.h
#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <stdint.h>

//function declarations
void chase_led();
void button_press(void(*callback)(void));
void EXTI0_IRQHandler(void);

// Sets the state of the LEDs
void set_led_state(uint16_t state);

// Gets the current state of the LEDs
void get_led_state(void);

#endif // DIGITAL_IO_H
