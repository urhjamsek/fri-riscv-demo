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

/* Board -- LEDs
 *   LD15..LD12   counter value, MSB on LD15 so it reads left to right
 *   LD1          BTNC toggle
 *   LD0          1 Hz software timer
 */
#define LED_BLINK           (1u << 0)        /* LD0                          */
#define LED_BUTTON          (1u << 1)        /* LD1                          */
#define LED_COUNT_SHIFT     12
#define LED_COUNT_MASK      (0xFu << LED_COUNT_SHIFT)

/* Board -- buttons. nexys_a7.xdc orders the GPIO channel C,U,L,R,D. */
#define BTN_CENTER          (1u << 0)        /* BTNC                         */
#define BTN_UP              (1u << 1)        /* BTNU -- increment            */
#define BTN_DOWN            (1u << 4)        /* BTND -- decrement            */
#define BTN_USED            (BTN_CENTER | BTN_UP | BTN_DOWN)

/* AXI UART Lite control bits (PG142) */
#define UART_CTRL_RST_TX    (1u << 0)
#define UART_CTRL_RST_RX    (1u << 1)
#define UART_CTRL_INTR_EN   (1u << 4)

/* AXI INTC (PG099) */
#define INTC_BASE           0x10060000UL

#define INTC_ISR            0x00    /* raw status, write 1 to clear      */
#define INTC_IPR            0x04    /* pending = ISR & IER, read only    */
#define INTC_IER            0x08    /* per-input enable                  */
#define INTC_IAR            0x0C    /* acknowledge, write the bit back   */
#define INTC_MER            0x1C    /* master enable                     */

#define INTC_MER_ME         (1u << 0)   /* master enable                 */
#define INTC_MER_HIE        (1u << 1)   /* hardware interrupts enable    */

/* AXI IIC (PG090) -- onboard ADT7420 temperature sensor */
#define IIC_BASE            0x10040000UL
#define IIC_SOFTR           0x040   /* write 0x0A to reset            */
#define IIC_CR              0x100
#define IIC_SR              0x104
#define IIC_TX_FIFO         0x108
#define IIC_RX_FIFO         0x10C
#define IIC_RX_FIFO_PIRQ    0x120
#define IIC_CR_EN           (1u << 0)
#define IIC_CR_TX_RST       (1u << 1)
#define IIC_SR_BB           (1u << 2)   /* bus busy                   */
#define IIC_SR_RX_EMPTY     (1u << 6)
#define IIC_SR_TX_FULL      (1u << 4)
/* Dynamic mode: these ride along with the byte written to TX_FIFO. */
#define IIC_START           (1u << 8)
#define IIC_STOP            (1u << 9)

/* AXI Quad SPI (PG153) -- onboard ADXL362 accelerometer */
#define QSPI_BASE           0x10050000UL
#define QSPI_SRR            0x40    /* write 0x0A to reset            */
#define QSPI_CR             0x60
#define QSPI_SR             0x64
#define QSPI_DTR            0x68
#define QSPI_DRR            0x6C
#define QSPI_SSR            0x70
#define QSPI_CR_LOOP        (1u << 0)
#define QSPI_CR_SPE         (1u << 1)   /* enable                     */
#define QSPI_CR_MASTER      (1u << 2)
#define QSPI_CR_TX_RST      (1u << 5)
#define QSPI_CR_RX_RST      (1u << 6)
#define QSPI_CR_MSS         (1u << 7)   /* manual slave select        */
#define QSPI_CR_INHIBIT     (1u << 8)
#define QSPI_SR_RX_EMPTY    (1u << 0)
#define QSPI_SR_TX_FULL     (1u << 3)

/* INTC inputs, in PERIPHS table order */
#define IRQ_UART            0
#define IRQ_BTN             1
#define IRQ_IIC             2
#define IRQ_QSPI            3
#define IRQ_UART_MASK       (1u << IRQ_UART)
#define IRQ_BTN_MASK        (1u << IRQ_BTN)
#define IRQ_IIC_MASK        (1u << IRQ_IIC)
#define IRQ_QSPI_MASK       (1u << IRQ_QSPI)

/* mie bits */
#define MIE_MEIE            (1u << 11)

static inline void     reg_wr(uint32_t base, uint32_t off, uint32_t v) { *(volatile uint32_t *)(base + off) = v; }
static inline uint32_t reg_rd(uint32_t base, uint32_t off)             { return *(volatile uint32_t *)(base + off); }

#endif
