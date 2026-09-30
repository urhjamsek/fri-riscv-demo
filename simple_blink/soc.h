/*=============================================================================
 * soc.h -- memory map and peripheral registers for the greenhouse SoC.
 *
 * Addresses come from soc/bd/create_bd.tcl; register offsets from the AMD IP
 * product guides (PG144 AXI GPIO, PG142 AXI UART Lite) and the CLINT header.
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

/* mie bits */
#define MIE_MEIE            (1u << 11)

static inline void     reg_wr(uint32_t base, uint32_t off, uint32_t v) { *(volatile uint32_t *)(base + off) = v; }
static inline uint32_t reg_rd(uint32_t base, uint32_t off)             { return *(volatile uint32_t *)(base + off); }

#endif
