#include "soc.h"
#include "uart.h"

void uart_hw_init(void)
{
    reg_wr(UART_BASE, UART_CTRL, UART_CTRL_RST_TX | UART_CTRL_RST_RX);
}

void uart_rx_irq_enable(void)
{
    reg_wr(UART_BASE, UART_CTRL, UART_CTRL_INTR_EN);
}

int uart_tx_full(void)
{
    return (reg_rd(UART_BASE, UART_STAT) & UART_STAT_TX_FULL) != 0;
}

void uart_tx_byte(char c)
{
    reg_wr(UART_BASE, UART_TX_FIFO, (uint8_t)c);
}

int uart_rx_ready(void)
{
    return (reg_rd(UART_BASE, UART_STAT) & UART_STAT_RX_VALID) != 0;
}

char uart_rx_byte(void)
{
    return (char)(reg_rd(UART_BASE, UART_RX_FIFO) & 0xFF);
}
