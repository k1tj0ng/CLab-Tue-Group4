#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER


#include <stdint.h>
#include "stm32f303xc.h"

// Defining the serial port struct, the definition is hidden in the
// c file as no one really needs to know this.
struct _SerialPort;
typedef struct _SerialPort SerialPort;


// make any number of instances of the serial port (they are extern because
//   they are fixed, unique values)
extern SerialPort USART1_PORT;


// The user might want to select the baud rate
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

 
// SerialInitialise - initialise the serial port
// Input: baud rate as defined in the enum
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t) );
 

// SerialOutputChar - output a char to the serial port
//  note: this version waits until the port is ready (not using interrupts)
// Input: char to be transferred
void SerialOutputChar(uint8_t, SerialPort *serial_port);
 

// SerialOutputString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void SerialOutputString(uint8_t *pt, SerialPort *serial_port);

char SerialInputChar(SerialPort *serial_port);

void SerialInputString(char *buffer, char terminatingChar, uint32_t max_length, SerialPort *serial_port);
 
// Serial call back stuff
typedef void (*SerialRxCallback)(char* str, uint32_t len);  // RX complete callback

void SerialSetRxCallback(SerialRxCallback cb);  // Register callback
void SerialEnableRxInterrupt(USART_TypeDef *USARTx, char terminatingChar);
void USART1_IRQHandler(void);  // ISR for USART1        // Set TX callback
//
#endif
