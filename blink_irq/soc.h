/*=============================================================================
 * soc.h -- memory map, peripheral registers and board wiring.
 *
 * Base addresses and interrupt numbers must match the PERIPHS table in
 * SoC/bd/create_bd.tcl (fri-riscv-core). Register offsets are from the AMD
 * product guides for each IP.
 *===========================================================================*/
#ifndef SOC_H
#define SOC_H

#include <stdint.h>

#define SOC_CLK_HZ          40000000UL
#define SOC_MTIME_HZ        1000000UL

#define RAM_BASE            0x80000000UL
#define RAM_SIZE            0x00020000UL     /* 128 KB */

#define CLINT_BASE          0x10000000UL
#define UART_BASE           0x10010000UL
#define GPIO_OUT_BASE       0x10020000UL     /* LEDs   (16 out)          */
#define GPIO_IN_BASE        0x10030000UL     /* buttons (5 in, IRQ)      */

/* CLINT (SiFive layout) */
#define CLINT_MSIP          (CLINT_BASE + 0x0000)
#define CLINT_MTIMECMP      (CLINT_BASE + 0x4000)
#define CLINT_MTIME         (CLINT_BASE + 0xBFF8)

/* AXI UART Lite */
#define UART_RX_FIFO        0x00
#define UART_TX_FIFO        0x04
#define UART_STAT           0x08
#define UART_CTRL           0x0C
#define UART_STAT_RX_VALID  (1u << 0)
#define UART_STAT_TX_FULL   (1u << 3)

/* AXI GPIO (v2) */
#define GPIO_DATA           0x000
#define GPIO_TRI            0x004
#define GPIO_GIER           0x11C
#define GPIO_IP_ISR         0x120
#define GPIO_IP_IER         0x128
#define GPIO_GIER_ENABLE    (1u << 31)
#define GPIO_CH1_IRQ        (1u << 0)

/* Board */
#define LED_BLINK           (1u << 0)        /* LD0 -- free-running blink   */
#define LED_BUTTON          (1u << 1)        /* LD1 -- toggled by BTNC IRQ  */
#define BTN_CENTER          (1u << 0)        /* BTNC, GPIO_IN bit 0         */

/* AXI INTC (PG099) */
#define INTC_BASE           0x10060000UL

#define INTC_ISR            0x00    /* raw status, write 1 to clear      */
#define INTC_IPR            0x04    /* pending = ISR & IER, read only    */
#define INTC_IER            0x08    /* per-input enable                  */
#define INTC_IAR            0x0C    /* acknowledge, write the bit back   */
#define INTC_MER            0x1C    /* master enable                     */

#define INTC_MER_ME         (1u << 0)   /* master enable                 */
#define INTC_MER_HIE        (1u << 1)   /* hardware interrupts enable    */

/* INTC inputs, in PERIPHS table order */
#define IRQ_UART            0
#define IRQ_BTN             1
#define IRQ_UART_MASK       (1u << IRQ_UART)
#define IRQ_BTN_MASK        (1u << IRQ_BTN)

/* mie bits */
#define MIE_MEIE            (1u << 11)

static inline void     reg_wr(uint32_t base, uint32_t off, uint32_t v) { *(volatile uint32_t *)(base + off) = v; }
static inline uint32_t reg_rd(uint32_t base, uint32_t off)             { return *(volatile uint32_t *)(base + off); }

#endif
