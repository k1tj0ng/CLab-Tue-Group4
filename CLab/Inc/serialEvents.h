#ifndef SERIAL_EVENT_HEADER
#define SERIAL_EVENT_HEADER

void Serial_ProcessRxEvent(char* buf, uint32_t len);

void Serial_ProcessTxEvent(void);

#endif
