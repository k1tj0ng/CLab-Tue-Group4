// digital_io.h

#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <stdint.h>

// Function declarations
void chase_led(void);
void button_press(void (*callback)(void));
void EXTI0_IRQHandler(void);

// Sets the state of the LEDs
void set_led_state(uint16_t state);

// Gets the current state of the LEDs
uint16_t get_led_state(void);  // Should return the value instead of void

// External variables
extern void (*on_button_press)(void);
extern uint16_t led_state;

#endif // DIGITAL_IO_H
