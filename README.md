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

### Valid input

### Functions and modularity
#### 1. Interrupts
The first thing that we need to do when setting up interrupts is to enable the peripherals needed. Afterwards, we need to set a specific interrupt function. This basically means telling the controller to generate an interrupt when a specific event that we set occurs. In this case, it is when we receive data in the UART module.
```
void enableInterrupts() {
  // Interrupts disabled so that process is not intervened
	__disable_irq();

	// Generates interrupt when data received
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
void USART1_EXTI25_IRQHandler() {
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
		unsigned char data = (uint8_t) USART1->RDR;

		// Store the read data
		string[i] = data;
		i++;

		// Toggle LEDs
		uint8_t* lights = ((uint8_t*)&(GPIOE->ODR)) + 1;
		*lights = !(*lights);
	}

} 
```
This function, called the Interrupt Service Routine (ISR), essentialy handles the event. When it is called, the received data will then be stored in a buffer storage. Afterwards, it transmits the data back to the user.

#### 2. Outputs
To output the function, we used a function called SerialOutputString and SerialOutputChar (from the lecture) that takes in the list of characters (array) as parameters and outputs the list to PuTTy or other terminal software.
```
code in here
```
We modified the function as well so that it can count the total length of the data received and output it.
```

```

#### 1. Interrupts
The first thing that we need to do when setting up interrupts is to enable the peripherals needed. Afterwards, we need to set a specific interrupt function. This basically means telling the controller to generate an interrupt when a specific event that we set occurs. In this case, it is when we receive data in the UART module.
```
void enableInterrupts() {
  // Interrupts disabled so that process is not intervened
	__disable_irq();

	// Generates interrupt when data received
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
void USART1_EXTI25_IRQHandler() {
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
		unsigned char data = (uint8_t) USART1->RDR;

		// Store the read data
		string[i] = data;
		i++;

		// Toggle LEDs
		uint8_t* lights = ((uint8_t*)&(GPIOE->ODR)) + 1;
		*lights = !(*lights);
	}

} 
```

#### 2. Outputs

### Testing
Testing is done through the use of serial terminal emulators such as PuTTY or CuteCom. After setting it up, we can check if data is received in the UART by debugging the code and stepping through it line by line.
```
code in here
```

### Usage (Instructions)
To start, set up the serial terminal emulator and connect it with your microcontroller. Then type in anything to the emulator. When finished, type in the terminating character. This will then be received by the microcontroller and it will be transmitted back to the emulator, along with the length of the string that you typed in. 

### Notes

## Exercise 3 - Timer Interfaces

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes

## Exercise 4 - Integration

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes
