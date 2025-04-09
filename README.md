<h1 align="center">MTRX2700 Project 2</h1>

<div align="center">
  Tue - Group 4 | C Language | STM32 | Mechatronics 2 - 2025 
</div>

---

## Justin, Kit, Ribka, Sulfat

## 👥 Roles and Responbilities
- **Justin:** Exercise 2: Serial Interfaces
- **Kit:** Exercise 2: Serial Interfaces
- **Ribka:** Exercise 3: Timer Interface
- **Sulfat:** Exercise 1: Digital I/O
- **Everyone contributed to Exercise 4: Integration.** 



## 🔍 Project Overview
Using C to interact with the microcontroller. Unlike last project, we aim to tackle the problems using interrupts instead of polling. This provides a more efficient way of processing our programs.


## Exercise 1 - Digital I/0

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes

## Exercise 2 - Serial Interfaces

### Summary Overview
This exercise involves using the UART ports to receive and transmit data. It has a similar concept with the assembly code but using C allows us to use a different approach to check the data registers through the use of interrupts. Unlike polling, which loops to check the register values, these interrupts allows us to sort of multitask by pausing its current work and jumps into a specific function (ISR). We will use this concept to receive and transmit data given to the UART.

### Usage
For part a and b of this exercise, we use a polling approach to receive the data being sent over the serial port.
```
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
This function is not really efficient since it loops to check, if the controller is ready to receive data or not. Hence, in part c, we changed the serial receiving process with an interrupt based approach.

### Functions and modularity
#### 1. Interrupts
The first thing that we need to do when setting up interrupts is to enable the peripherals needed. Afterwards, we need to set a specific interrupt function. This basically means telling the controller to generate an interrupt when a specific event that we set occurs. In this case, it is when we receive data in the UART module.
```
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
Now, whenever the receiving bit of the UART sends a flag saying that it has received data, the interrupt is generated and the controller will automatically jump to the interrupt handler function shown below.
```
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
//	SerialOutputChar(data, &USART1_PORT);

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
This function, called the Interrupt Service Routine (ISR), essentialy handles the event. When it is called, the received data will then be stored in a buffer storage. Afterwards, it transmits the data back to the user.

#### 2. Outputs
To output the function, we used a function called SerialOutputString and SerialOutputChar (from the lecture) that takes in the list of characters (array) as parameters and outputs the list to PuTTy or other terminal software.
```
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
Testing is done through the use of serial terminal emulators such as PuTTY or CuteCom. After setting it up, we can check if data is received in the UART by debugging the code and stepping through it line by line.
We can also print out statements in the serial port by using our serial output function in certain blocks to check if the flow is running correctly. The example is shown below
```
code in here (put the one for error checking like SerialOutputString("message received)
```

### Usage (Instructions)
To start, set up the serial terminal emulator and connect it with your microcontroller. Then type in anything to the emulator. When finished, type in the terminating character. This will then be received by the microcontroller and it will be transmitted back to the emulator, along with the length of the string that you typed in. 

### Notes

## Exercise 3 - Timer Interfaces

### Summary
This section is the system's timer software module. The timer should trigger a callback function at regular intervals, allow resetting the interval, and trigger a one-shot event. The module is designed using a general-purpose timer (TIM2).

### Usage
The timer module allows you to:
1. **Initialize the timer** to trigger a callback function at a regular interval.
2. **Reset the timer** with a new period when needed.
3. **Trigger a one-shot event**, where the timer triggers a callback once and then stops.

### Valid input
1. **Interval for Periodic Timer**
   The `interval` parameter passed to the `timer_init` function specifies the time period in milliseconds at which the callback will be executed. Valid inputs are positive integer values that represent the time in milliseconds.
2. **Delay for One-Shot Timer**
   The `delay` parameter passed to the `timer_one_shot` function specifies the time in milliseconds before triggering the callback once. It should also be a positive integer.

### Functions and modularity
1. **`timer_init(uint32_t interval, CallbackFunction callback)`**:
   - This function initializes the timer with a specified interval (in milliseconds) and a callback function.
   - It sets up the timer’s prescaler, auto-reload register (ARR), and enables the timer interrupt.
   - The callback function is stored for later execution when the timer overflows.

2. **`timer_reset(uint32_t interval)`**:
    - This function resets the timer with a new interval.
    - It resets the counter, updates the ARR register, and re-starts the timer.

3. **`timer_one_shot(uint32_t delay, CallbackFunction callback)`**:
    - This function sets the timer for a one-shot event, meaning the timer triggers the callback once after the specified delay and then stops.
    - It uses a flag (`isOneShot`) to indicate that the timer should stop after one callback.
      
4. **`TIM2_IRQHandler(void)`**:
    - This interrupt handler is triggered whenever the timer (TIM2) overflows.
    - It checks the interrupt flag, clears it, and executes the stored callback function.
    - If the one-shot mode is active, it disables the timer after executing the callback to stop it from continuing.

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
Using the previous exercises, the integration task focuses on combining all the modules to develop a program that can perform several functions, depending on the user input to the serial port. There are several pre-defined commands that the user can type into the serial port such as, 'led 10001010', 'serial This transmits the strings back', 'oneshot 1000', and 'timer 1000'. We will look at this in more detail. We can also think of this like calling a function with custom parameters through the serial port. The program checks for which function is to be called with the use of a if-else block.

### Usage
The user should type a string with 2 parts, first being the function name and second being the function value (parameters). It is then received by the UART receive module. The program will then extract the first part of the string and checks which function to be called and passes on the parameters which is the second part of the string. Both parts of the strings will be stores in seperate buffers to be passed/called by the following functions. After storing the first part of the string, we can then use an if-else block to check which function needs to be called.
```
// 'command' is where the first part of the string is stored
// 'value' is where the second part of the string is stored
if(strcasecmp(command, "led") == 0) {
handleNumericCommand("LED", value);
} else if(strcasecmp(command, "timer") == 0) {
        handleNumericCommand("TIMER", value);
} else if(strcasecmp(command, "oneshot") == 0) {
        handleNumericCommand("ONESHOT", value);
} else if(strcasecmp(command, "serial") == 0) {
        handleSerial(value);
} else {
        SerialOutputString((uint8_t*)"Unknown command: ", &USART1_PORT);
        SerialOutputString((uint8_t*)command, &USART1_PORT);
        SerialOutputString((uint8_t*)"\n", &USART1_PORT);
}
```

#### 1. LED 
Taking the values from the second part of the string, i.e. 10001010, the function should output LEDs with the given pattern value. 
```
code in here
```

#### 2. Serial
From exercise 2, the "serial" comand will transmit the string back over the serial port. 
```
code in here
```

#### 3. Oneshot


#### 4. Timer

### Valid input
- The input by the user should have 2 parts.
- The first part must be in lower case.
- The second part must correspond to the parameters of the function to be called.

### Functions and modularity
#### 1. Enable Interrupts
#### 2. Interrupt Handlers
#### 3. LEDs Registers
#### 4. Serial Registers
#### 5. Timers Registers

### Testing

### Notes
