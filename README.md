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

### Summary
This exercise involves using the UART ports to receive and transmit data. It has a similar concept with the assembly code but using C allows us to use a different approach to check the data registers through the use of interrupts. Unlike polling, which loops to check the register values, these interrupts allows us to sort of multitask by pausing its current work and jumps into a specific function (ISR). In this exercise, we set the interrupt condition to be whenever we receive a data in the UART (USART_CR1_RXNEIE bit is set). It will then jump into a function to output what it receives, along with the length of the data.

### Usage

### Valid input

### Functions and modularity

#### 1. Interrupts


#### 2. Outputs

### Testing

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
