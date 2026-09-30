/*=============================================================================
 * temp.c -- ADT7420 temperature sensor over AXI IIC (PG090), polled.
 *
 * The IIC core runs in dynamic mode: each byte written to the TX FIFO can
 * carry a start or stop flag. A register read is
 *     start, address + write, register, repeated start, address + read,
 *     byte count + stop
 *
 * Not thread-safe; used by one task. Every wait has a timeout, so a missing
 * sensor returns -1 instead of hanging.
 *===========================================================================*/
#include "soc.h"
#include "temp.h"

#define ADT7420_ADDR    0x4B
#define REG_TEMP_MSB    0x00
#define REG_ID          0x0B
#define ADT7420_ID      0xCB

#define SPIN            200000      /* poll limit */

static int wait_clear(uint32_t off, uint32_t mask)
{
    for (int i = 0; i < SPIN; i++) {
        if ((reg_rd(IIC_BASE, off) & mask) == 0) return 0;
    }
    return -1;
}

static void bus_reset(void)
{
    reg_wr(IIC_BASE, IIC_SOFTR, 0x0A);
    for (volatile int i = 0; i < 1000; i++) { }
    reg_wr(IIC_BASE, IIC_CR, IIC_CR_TX_RST);
    reg_wr(IIC_BASE, IIC_CR, IIC_CR_EN);
}

/* Read n bytes starting at reg. 0 on success. */
static int rd(uint8_t reg, uint8_t *out, int n)
{
    if (wait_clear(IIC_SR, IIC_SR_BB) < 0) { bus_reset(); return -1; }

    /* part one: start, address for writing, the register number */
    reg_wr(IIC_BASE, IIC_TX_FIFO, IIC_START | (ADT7420_ADDR << 1));
    reg_wr(IIC_BASE, IIC_TX_FIFO, reg);

    /* part two: repeated start, address for reading, byte count with stop */
    reg_wr(IIC_BASE, IIC_TX_FIFO, IIC_START | (ADT7420_ADDR << 1) | 1u);
    reg_wr(IIC_BASE, IIC_TX_FIFO, IIC_STOP  | (uint32_t)n);

    for (int i = 0; i < n; i++) {
        int spin = SPIN;
        while (reg_rd(IIC_BASE, IIC_SR) & IIC_SR_RX_EMPTY) {
            if (--spin <= 0) { bus_reset(); return -1; }
        }
        out[i] = (uint8_t)(reg_rd(IIC_BASE, IIC_RX_FIFO) & 0xFF);
    }
    return 0;
}

int temp_init(void)
{
    uint8_t id;

    bus_reset();
    if (rd(REG_ID, &id, 1) < 0) return -1;
    return (id == ADT7420_ID) ? 0 : -1;
}

int temp_read(int *tenths)
{
    uint8_t  b[2];
    int16_t  raw;

    if (rd(REG_TEMP_MSB, b, 2) < 0) return -1;

    /* 13-bit signed value, left aligned, in 1/16 degree */
    raw = (int16_t)(((uint16_t)b[0] << 8) | b[1]);
    raw >>= 3;

    /* 1/16 -> 1/10 degree, rounded, integer only */
    *tenths = ((int)raw * 10 + (raw >= 0 ? 8 : -8)) / 16;
    return 0;
}
