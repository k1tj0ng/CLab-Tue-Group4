// Separate module to handle callback logic
#include <stdint.h>

void Serial_ProcessRxEvent(char* buf, uint32_t len) {
    if(user_rx_cb) user_rx_cb(buf, len);
}

void Serial_ProcessTxEvent(void) {
    if(user_tx_cb) user_tx_cb();
}
