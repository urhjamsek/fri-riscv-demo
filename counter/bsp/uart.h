#ifndef UART_H
#define UART_H
#include <stdint.h>

/* AXI UART Lite, register level. No buffering and no locking. */

void uart_hw_init(void);         /* flush both FIFOs, interrupt still off  */
void uart_rx_irq_enable(void);   /* raise an interrupt when a byte arrives */

int  uart_tx_full(void);
void uart_tx_byte(char c);

int  uart_rx_ready(void);
char uart_rx_byte(void);

#endif
