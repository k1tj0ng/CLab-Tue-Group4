<h1 align="center">MTRX2700 Project 2</h1>

<div align="center">
  Tue - Group 4 | C Language | STM32 | Mechatronics 2 - 2025 
</div>

---

## Justin, Kit, Ribka, Sulfat

## 👥 Roles and Responbilities
- **Justin:** Majority of work on README, contributed to Exercise 2: Serial Interfaces
- **Kit:** Majority of implementation for Exercise 4: Integration, also worked on Exercise 2: Serial Interfaces
- **Ribka:** Main contributor to Exercise 3: Timer Interface
- **Sulfat:** Main contributor to Exercise 1: Digital I/O
- **Everyone contributed to Exercise 4: Integration.** 


## 🔍 Project Overview
Using C to interact with the microcontroller. Unlike last project, we aim to tackle the problems using interrupts instead of polling. This provides a more efficient way of processing our programs.  


**Note:** All *header* files have been correctly initialised but not included in the documentation for simplicity.

# Getting Started


## Exercise 1 - Digital I/O

### Summary
This exercises involves the usage of General-Purpose Input/Output (GPIO) pins to turn off and on LEDs through user-input via a button-press. This is done through using interrupts. All functions for initialisation, controlling LED state and restricting the duration of delay, are contained in "**digital_io.c**", "**gpioe_config.c**", "**handler.c**", "**interrupts.c**", and "**timer.c**", and the functions are declared in their respective header files. All functions are called in the main function to maintain modularity.  
This exercise is based of the project **W05-C-interrupt**.

### Usage
To use this module:
1. Initialise interrupts, clocks and, GPIOs for LEDs output and button input. 
2. Call `button_press` and `LEDenableInterrupts` function to run when button is pressed where it generates an interrupt event.
3. LED will light up when the button is pressed.
4. Set integer number for delay.
5. Call `enable_prescalar` function to enable timer 2 interrupt.
6. Active LED will be shifted left by one bit every time the timer elapses which triggers an interrupt event. 

### Valid input
Input any integer number as the duration for the delay. 

### Functions and modularity
1. "**digital_io.c**"  

i. `button_press(void(*callback)(void))`
```cpp
void button_press(void(*callback)(void)){
	on_button_press= callback;
}
```
- This function is responsible for taking any user-defined callback function to be called when an interrupt event is triggered, i.e. button press.  

ii. `chase_led()`
```cpp
void chase_led(){
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	*led_register <<= 1;
	if (*led_register == 0) {
		*led_register = 1;
	}
}
```
- This function is responsible for shifting the LEDs by one bit to the left.

iii. `enable_prescaler(int delay_value)`
```cpp
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
```
- This function is responsible for initialising timer 2 and enabling the timer 2 interrupt.
- The variable `delay_value` defines how long it takes for the timer to elapse and trigger an interrupt event so that the active LED can be shifted to the left.

iv. `set_led_state(uint16_t state)`
```cpp
void set_led_state(uint16_t state) {
	led_state = state;
	enable_clocks();
	initialise_board();
	GPIOE->ODR &= ~(0xFF << 8);       // Clear bits 8–15 (LEDs)
	GPIOE->ODR |= (state << 8);       // Set new LED state

//	GPIOE->ODR = (GPIOE->ODR & 0x00FF) | (state << 8); // set led state
}
```
- This function is responsible for setting a user-defined 8-bit bitmask to the global `led_state` variable by clearing the previous LED state and setting the new one.

v. `get_led_state(void)`
```cpp
uint16_t get_led_state(void) {
    return (GPIOE->ODR >> 8) & 0xFF00;
}
```
- This function is responsible for retrieving the current state of the LEDs.

2. **Interrupts**
i. `LEDenableInterrupts()`
- The function to set/enable the interrupt.
- The code below sets the condition that if the button is pressed, an interrupt is triggered as it is set on rising edge mode.
```cpp
void LEDenableInterrupts() {
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
```
ii. `EXTI0_IRQHandler(void)`
- Interrupt handler function.
- This function is called when the interrupt is triggered.
```cpp
void EXTI0_IRQHandler(void)
{
	// run the button press handler (make sure it is not null first !)
	if (on_button_press != 0x00) {
		on_button_press();
	}

	// reset the interrupt (so it doesn't keep firing until the next trigger)
	EXTI->PR |= EXTI_PR_PR0;
}
```
- It essentially just calls the callback function and clear the interrupt flag.

### Testing
For testing and debugging, a few procedures were performed:
- First, for testing the interrupt, the button was pressed multiple times to observe the pattern that would emerge. The expected behaviour was that the LED would be shifted in a clockwise direction.
- Second, for the LED display, several different bitmasks were inputted to test whether the pattern would be displayed correctly, such as `0b1111111`.
### Notes

## Exercise 2 - Serial Interfaces

### Summary Overview
This exercise involves using the UART ports to receive and transmit data. It has a similar concept with the assembly code but using C allows us to use a different approach to check the data registers through the use of interrupts. We will use this concept to receive and transmit data given to the UART.  
This exercise is based of the project **W06-UART-modular-design**.

### Usage
The serial module allows you to:  
1. **Receive data** that is typed by the user from the serial emulator
2. **Store data** into a variable or buffer
3. **Transmit data** back to the serial port

### Functions and modularity
1. **Initialising GPIO and USART** 
```cpp
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {
	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// enable the GPIO which is on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// set pin mode to alternate function for the specific GPIO pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

	// enable high speed clock for specific GPIO pins
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to external pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// enable the device based on the bits defined in the serial port definition
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}
	// enable serial port for tx and rx
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}
```

2. **Receive Data**  
- For parts a and b of this exercise, we use a polling approach to receive the data being sent over the serial port.
```cpp
uint8_t SerialInputChar(SerialPort *serial_port) {
    // Wait until data is received
    while((serial_port->UART->ISR & USART_ISR_RXNE) == 0) {
    }

    // Read and return the received data
    return (uint8_t)serial_port->UART->RDR;
}

void SerialInputString(uint8_t *buffer, uint8_t terminator, SerialPort *serial_port) {
    uint32_t counter = 0;
    uint8_t received_char;

    do {
        // Read one character
        received_char = SerialInputChar(serial_port);

        // Store it in the buffer
        buffer[counter] = received_char;
        counter++;

        // Check if we've received the terminator
    } while(received_char != terminator);

    // Add null terminator to make it a proper C string
    buffer[counter] = '\0';

    // Call completion function if one exists
    if(serial_port->completion_function != 0) {
        serial_port->completion_function(counter);
    }
}
```
- This function is not really efficient since it loops to check, if the controller is ready to receive data or not.
- In part c, we changed the serial receiving process with an interrupt based approach.

3. **Interrupts**  
- Once we got our GPIO pins ready, we then set a specific interrupt function. This basically means telling the controller to generate an interrupt when a specific event that we set occurs. In this case, it is when we receive data in the UART module.
```cpp
void enableInterrupts() {
	// Interrupts disabled so that process is not intervened
	__disable_irq();

	// Generate an interrupt upon receiving data
	USART1->CR1 |= USART_CR1_RXNEIE_Msk;

	// Set priority and enable interrupts
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

  	// Interrupts enabled
	__enable_irq();
}
```
- Now, whenever the receiving bit of the UART sends a flag saying that it has received data, the interrupt is generated and the controller will automatically jump to the interrupt handler function shown below.
```cpp
void USART1_EXTI25_IRQHandler(void) {
    // Check for errors first (clears them automatically on read)
    if (USART1->ISR & (USART_ISR_FE | USART_ISR_ORE)) {
        USART1->ICR |= USART_ICR_FECF | USART_ICR_ORECF; // Clear flags
        return;
    }

    // Only handle RX not empty interrupt
    if (USART1->ISR & USART_ISR_RXNE) {
        uint8_t data = (uint8_t)(USART1->RDR & 0xFF);

        // Handle buffer overflow
        if (writePos >= BUFFER - 1) {
            writePos = 0;
            memset((void*)strings[activeIndex], 0, BUFFER);
            return;
        }

        // Echo back (optional)
	// SerialOutputChar(data, &USART1_PORT);

	// Double buffer in action
        if (data == '\n' || data == '\r') {
            // Terminate string and mark buffer ready
            strings[activeIndex][writePos] = '\0';
            bufferReady = 1;

            // Swap buffers
            readyIndex = activeIndex;
            activeIndex ^= 1;
            writePos = 0;

            // Clear new active buffer
            memset((void*)strings[activeIndex], 0, BUFFER);
        } else {
            // Store regular character
            strings[activeIndex][writePos++] = data;
        }
    }
}
```
- This function, called the Interrupt Service Routine (ISR), essentialy handles the event. When it is called, the received data will then be stored in a buffer storage. Afterwards, it transmits the data back to the user.  

**Advanced Functionality**:
- Double buffer used which allows the program to take in another string without intervening the interrupt process

4. **Outputs**
- To output the function, we used a function called SerialOutputString and SerialOutputChar (from the lecture) that takes in the list of characters (array) as parameters and outputs the list to PuTTy or other terminal software.
```cpp
void SerialOutputChar(uint8_t data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
	}

	serial_port->UART->TDR = data;
}



void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {

	uint32_t counter = 0;
	while(*pt) {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}

	serial_port->completion_function(counter);
}
```

### Testing
Testing is done through the use of serial terminal emulators such as PuTTY or CuteCom. After setting it up, type in anything to the emulator. When finished, type in the terminating character. 
- We can then check if data is received in the UART by debugging the code and stepping through it line by line.
- We can also print out statements in the serial port by using our serial output function in certain blocks to check if the flow is running correctly.
- To test the double buffer functionality, we simply just need to check if we can continuously type in the serial ports without needing to hit the reset button.


### Notes

## Exercise 3 - Timer Interfaces

### Summary
This section is the system's timer software module. The timer should trigger a callback function at regular intervals, allow resetting the interval, and trigger a one-shot event. The module is designed using a general-purpose timer (TIM2).  
This exercise is based on the project **W06-C-timers**.

### Usage
The timer module allows you to:
1. **Initialize the timer** to trigger a callback function at a regular interval.
2. **Reset the timer** with a new period when needed.
3. **Trigger a one-shot event**, where the timer triggers a callback once and then stops.

### Valid input
1. **Interval and new_interval for Periodic Timer**
   The `interval` and `new_interval` parameters passed to the `timer_init` or `timer_reset` function specify the time period in milliseconds at which the callback will be executed. Valid inputs are positive integer values that represent the time in milliseconds.
2. **Delay for One-Shot Timer**
   The `delay` parameter passed to the `timer_one_shot` function specifies the time in milliseconds before triggering the callback once. It should also be a positive integer.

### Functions and modularity
1. **`timer_init(uint32_t interval, CallbackFunction callback)`**:
   - This function initializes the timer with a specified interval (in milliseconds) and a callback function.
   - It sets up the timer’s prescaler, auto-reload register (ARR), and enables the timer interrupt.
   - The callback function is stored for later execution when the timer overflows.
```cpp
void timer_init(uint32_t interval, CallbackFunction callback) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop the timer
	isOneShot = false;		// Flag for periodic timer mode

	// Set up the timer prescaler, for 1khZ frequency = (8MHz:1000)-1
	TIM2->PSC = 7999;
	TIM2->ARR = interval;		// Set the time interval
	TIM2->CNT = 0;			// Reset the counter

	TIM2->DIER |= TIM_DIER_UIE;	// Enable interrupt when there is overflow
	NVIC_EnableIRQ(TIM2_IRQn);	// Enable interrupt in the NVIC
	
	timerCallback = callback;	// Set the callback function
	
	TIM2->CR1 |= TIM_CR1_CEN;	// Start the timer
}
```

2. **`timer_reset(uint32_t interval)`**:
    - This function resets the timer with a new interval.
    - It resets the counter, updates the ARR register, and re-starts the timer.
```cpp
void timer_reset(uint32_t new_interval) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop timer
	TIM2->CNT = 0;			// Reset counter
	TIM2->ARR = new_interval;	// Update ARR
	TIM2->EGR |= TIM_EGR_UG;	// Force update event to load ARR
	TIM2->CR1 |= TIM_CR1_CEN;	// Restart timer
}
```

3. **`timer_one_shot(uint32_t delay, CallbackFunction callback)`**:
    - This function sets the timer for a one-shot event, meaning the timer triggers the callback once after the specified delay and then stops.
    - It uses a flag (`isOneShot`) to indicate that the timer should stop after one callback.
```cpp
void timer_one_shot(uint32_t delay, CallbackFunction callback) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop timer
	isOneShot = true;		// Flag for oneshot timer mode

	// Set up the timer prescaler
	TIM2->PSC = 7999;
	TIM2->ARR = delay;
	TIM2->CNT = 0;

	// Enable interrupt
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);

	timerCallback = callback;

	TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
}
```
      
4. **`TIM2_IRQHandler(void)`**:
    - This interrupt handler is triggered whenever the timer (TIM2) overflows.
    - It checks the interrupt flag, clears it, and executes the stored callback function.
    - If the one-shot mode is active, it disables the timer after executing the callback to stop it from continuing.
```cpp
void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {		// Check if the interrupt flag is raised
		TIM2->SR &= ~TIM_SR_UIF;	// Clear the interrupt flag
		timerCallback();		// Execute the callback function

		// Check if one-shot mode
		if (isOneShot) {
			TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop the timer after the callback
			isOneShot = false;		// Clear the one-shot flag
		}
	}
}
```

### Testing
To test the functionality of the timer module, the following steps were used:
1. **Test Periodic Timer**:
   - Initialize the timer with a specific interval (e.g., 100 ms) and a callback function that swaps the state of the LED. Verify that the callback is executed at the expected interval.
   - Test with another time interval.
2. **Test Timer Reset**:
    - Use the periodic timer for a certain amount of time (use the for loop to waste some time)
    - Call `timer_reset` with a different interval and verify that the timer now triggers the callback at the new interval. Ensure that the timer continues to function after the reset.
3. **Test One-Shot Timer**:
    - Call `timer_one_shot` with a delay and verify that the callback is executed exactly once after the specified time. Ensure that the timer stops after triggering the callback and does not repeat.

### Notes
The timer interrupt is handled by the TIM2_IRQHandler function. The interrupt will only be triggered when the timer overflows.


## Exercise 4 - Integration

### Summary
Using the previous exercises, the integration task focuses on combining all the modules to develop a program that can perform several functions, depending on the user input to the serial port. There are several pre-defined commands that the user can type into the serial port such as, `led 10001010`, `serial This transmits the strings back`, `oneshot 1000`, and `timer 1000`. We will look at this in more detail. We can also think of this like calling a function with custom parameters through the serial port. The program checks for which function is to be called with the use of a if-else block.

### Notes
Most of the initialising functions in here are the same, therefore only some of the important parts are put into the code snippet to avoid repitition.

### Usage
The user should type a string with 2 parts, first being the function name and second being the function value (parameters). It is then received by the UART receive module. The program will then extract the first part of the string and checks which function to be called and passes on the parameters which is the second part of the string. Both parts of the strings will be stores in seperate buffers to be passed/called by the following functions. After storing the first part of the string, we can then use an if-else block to check which function needs to be called.

The commands are more or less the same as exercises 1, 2, and 3. Here is a brief recap of what each command should do:

1. LED 
- Taking the values from the second part of the string, i.e. 10001010, the function should output LEDs with the given pattern value. 

2. Serial
- From exercise 2, the "serial" comand will transmit the received string back over the serial port.  

3. Oneshot
- The timer should run the callback function only once after a set specified delay.  

4. Timer
- the timer would do a task at every regular interval.

### Valid input
- The input by the user should have 2 parts.
- The first part should be a function name listed in the usage above.
- The second part should correspond to the parameters of the function to be called.

### Functions and modularity
#### 1. "**integration.c**"
This module is designed to handle the main process of the program with the use of several functions. It branches to other function depending on the string entered by the user in the serial port. Some modules are directly taken from previous exercise, resulting in no change within the module.  
We added a global variable:  
```cpp
int INTERVAL_BUFFER = 10;
```

i. **`sortingOutInput(char buffers[][BUFFER], uint8_t bufIndex)`**
- This function's task is to read the user's input and decide which function is to be called next
- The following code snippets will be divided into parts for simplicity
```cpp
void sortingOutInput(char buffers[][BUFFER], uint8_t bufIndex) {
    char* input = buffers[bufIndex];

    // Trim whitespace and newlines
    size_t len = strlen(input);
    while(len > 0 && isspace(input[len-1])) {
        input[--len] = '\0';
    }

    // Skip leading whitespace
    char* cmdStart = input;
    while(isspace(*cmdStart)) {
        cmdStart++;
    }
```
- The code above essentially cleans the string received by removing any trailing/leading whitespaces and sorts.
- It also adds a null terminating character `\0` at the end of the string.
```cpp
    // Extract command and value
    char* command = cmdStart;
    char* value = strchr(cmdStart, ' ');
    if(value != NULL) {
        *value++ = '\0';
        while(isspace(*value)) value++;
    }
```
- This part of the code is where the user's input is assigned into a variable.
- `command` refers to the first part of the string.
- `value` refers to the second part of the string.

```cpp
    // Debug print
	SerialOutputString((uint8_t*)"\nProcessing: [", &USART1_PORT);
	SerialOutputString((uint8_t*)cmdStart, &USART1_PORT);
	SerialOutputString((uint8_t*)"]\n", &USART1_PORT);
```
- To make the process cleaner, a statement is printed through the serial port.

```cpp
    // Command processing
    if(strcasecmp(command, "led") == 0) {
        handleNumericCommand("LED", value);
        uint8_t bitmask = strtol(value, NULL, 2);
        set_led_state(bitmask);
    }
    else if(strcasecmp(command, "timer") == 0) {
    	handleNumericCommand("TIMER", value);
    	uint32_t interval = strtol(value, NULL, INTERVAL_BUFFER);
    	timerhandle(interval);
    }
    else if(strcasecmp(command, "oneshot") == 0) {
    	handleNumericCommand("ONESHOT", value);
    	uint32_t delay = strtol(value, NULL, INTERVAL_BUFFER);
    	timer_one_shot(delay, blink_leds);
    //	timer_one_shot(value, timerCallback);
    }
    else if(strcasecmp(command, "serial") == 0) {
        handleSerial(value);
    }
    else {
        SerialOutputString((uint8_t*)"Unknown command: ", &USART1_PORT);
        SerialOutputString((uint8_t*)command, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
    SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    memset(buffers[bufIndex], 0, BUFFER);
```
- Using if-else statements, we then compare `command` with several different keywords that refers to the function it should call afterwards.
- Once it matches one of the keywords, it calls the designated function, taking `value` as a parameter.

ii. **`handleNumericCommand(const char* name, const char* value)`**
- From our sorting function, we get a variable `value` which should be the second part of the user's input.
- If `command` is either "led", "timer", or "oneshot", then this function is called to handle `value`.
```cpp
void handleNumericCommand(const char* name, const char* value) {
    if(value == NULL) {
        char msg[BUFFER];
        sprintf(msg, "Error: %s needs value\n", name);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
        return;
    }
```
- The if block statement above ensures that the variable `value` has data in it.
- If the user's input does not have 2 parts, then automatically, `value` is set to NULL.
- If so, then an error is printed into the serial output.

```cpp
    char* endptr;
    long num = strtol(value, &endptr, 10);
```
- This section basically converts the string into a long integer in decimals.
  
```cpp
    if(*endptr != '\0' || num < 0) {
        SerialOutputString((uint8_t*)"Error: Invalid number\n", &USART1_PORT);
    } else {
        char msg[BUFFER];
        sprintf(msg, "%s: %ld\n", name, num);
        SerialOutputString((uint8_t*)msg, &USART1_PORT);
    }
}
```
- The last if-else statement checks if the string has succesfully been converted and if the numerical value is logical or not.
- If so, it then outputs the command and value to the serial output.

iii. **`handleSerial(const char* value)`**
- This function is called when `command` is "serial"
```cpp
void handleSerial(const char* value) {
    if(value == NULL) {
        SerialOutputString((uint8_t*)"Error: SERIAL needs string\n", &USART1_PORT);
        return;
    }
```
- Similar to `handleNumericCommand` function, the if statement above, checks if `value` has data.
- If not, it prints an error message to the serial output.
```cpp
    if(strlen(value) > BUFFER) {
        SerialOutputString((uint8_t*)"Error: Max 32 chars\n", &USART1_PORT);
    } else {
        SerialOutputString((uint8_t*)"SERIAL: ", &USART1_PORT);
        SerialOutputString((uint8_t*)value, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
    }
}
```
- Afterwards, it checks if the length of the string is within a specified value.
- If it is, it can then print out the string stored in `value` to the serial output, effectively transmitting the user's input back.


#### 2. "**interrupts.c**"
This module contains functions that are used to enable the interrupts which are taken from the previous exercise. There is no change to the content of the functions.  
i. `UARTenableInterrupts()`  
ii. `LEDenableInterrupts()`

#### 3. "**handler.c**"
For this specific integrated program, we introduce new global variables:
```cpp
volatile uint16_t writePos = 0;       // Current write position
volatile bool bufferReady = false;     // Data ready flag
volatile bool termminatingCharDetected = false; 
```
`bufferReady` will then be used later on in the main function as a flag. 

i. **`USART1_EXTI25_IRQHandler(void)`**
- Taken from previous exercises, the handler function only differs in the data checking part.
```cpp
if (data == '\n' || data == '\r') {
	// Terminate string and mark buffer ready
	strings[activeIndex][writePos] = '\0';
	bufferReady = 1;

	// Swap buffers
	readyIndex = activeIndex;
	activeIndex ^= 1;
	writePos = 0;

	// Clear new active buffer
	memset((void*)strings[activeIndex], 0, BUFFER);
}
```
- The code above checks if the received string is `\n` or `\r` as it signals the end of a message.
- If so, `bufferReady` is set to 1, signaling that it is ready to be processed in the main function.
- We also added in the terminatingCharDetected variable which is used in the function.
```cpp
else if (data == TERMINATING_CHAR) {
	terminatingCharDetected = 1;
	SerialOutputString((uint8_t *)"\nTERMINATING CHARACTER DETECTED!", &USART1_PORT);
```

#### 4. "**timer.c**"
- We added in a new function to make the timer work in the integration.  
i. `timerhandle(int interval)`
```cpp
void timerhandle(int interval) {
    static bool timerRunning = false;

    if (!timerRunning) {
        timer_init(interval, blink_leds);
        timerRunning = true;
    } else {
        timer_reset(interval);
    }
}
```

#### 5. "**digital_io.c**"
- We made some minor change in our set led function which involves clearing the timer flag so that the timer will not intervene the LED process.  
i. `set_led_state(uint8_t state)`
```cpp
void set_led_state(uint8_t state) {
	TIM2->CR1 &= ~TIM_CR1_CEN;	  // Clear timer flag
	GPIOE->ODR &= ~(0xFF << 8);       // Clear bits 8–15 (LEDs)
	GPIOE->ODR |= (state << 8);       // Set new LED state
}
```

#### 6. "**gpioe_config.c**"
- Taken from previous exercises, there is no change in the function.

#### 7. "**serial.c**"
- Taken from previous exercises, the contents of this module is not changed.

## Instructions for User
1. __Install Required Software__:
- Install [CuteCom](https://cutecom.sourceforge.net) and [cMaker](https://cmake.org/download/). Configure CuteCom to connect to the port with a baud rate of 115200, 8 data bits, 1 stop bit, no parity, and no flow control.
- Install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) to transfer code to the STM32 board.

2. __Code Setup__:
- Clone the repo.
- Import Clab folder into STM32IDE workspace.
- Run Clab debug.
- Open CuteCom and enter prompt in the terminal.

***Special Functionality***:  
Once the custom terminating character has been entered, the program will stop operating.

## Details About Testing Procedures
**Troubleshooting**
1. If no output is visible in the terminal after transmitting the string of characters: check if buffer being initialise properly, confirm CuteCom is configure correctly.
2. If none of the above work, function logic, base addresses and registers used should be checked.

**General Testing Strategy:** Debugging in the IDE while checking register and memory browser.
