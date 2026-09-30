/*=============================================================================
 * accel.c -- ADXL362 accelerometer over AXI Quad SPI (PG153), polled.
 *
 * SPI mode 0. A register read sends 0x0B, the register, then one padding
 * byte per byte to read. Chip select must stay low for the whole transfer,
 * so it is driven manually (CR.MSS, SSR).
 *
 * Not thread-safe; used by one task. Every wait has a timeout.
 *===========================================================================*/
#include "soc.h"
#include "accel.h"

#define CMD_WRITE   0x0A
#define CMD_READ    0x0B
#define REG_DEVID   0x00
#define REG_XDATA   0x08
#define REG_POWER   0x2D
#define POWER_MEAS  0x02
#define DEVID_AD    0xAD

#define SPIN        100000

/* Enabled, master, manual slave select, transfers inhibited. */
#define CR_IDLE     (QSPI_CR_SPE | QSPI_CR_MASTER | QSPI_CR_MSS | QSPI_CR_INHIBIT)

static void bus_reset(void)
{
    reg_wr(QSPI_BASE, QSPI_SRR, 0x0A);
    for (volatile int i = 0; i < 1000; i++) { }
    reg_wr(QSPI_BASE, QSPI_SSR, 0xFFFFFFFFu);           /* deselect */
    reg_wr(QSPI_BASE, QSPI_CR, CR_IDLE | QSPI_CR_TX_RST | QSPI_CR_RX_RST);
    reg_wr(QSPI_BASE, QSPI_CR, CR_IDLE);
}

/* Shift len bytes out of tx and len bytes back into rx. 0 on success. */
static int xfer(const uint8_t *tx, uint8_t *rx, int len)
{
    reg_wr(QSPI_BASE, QSPI_CR, CR_IDLE | QSPI_CR_TX_RST | QSPI_CR_RX_RST);
    reg_wr(QSPI_BASE, QSPI_CR, CR_IDLE);

    for (int i = 0; i < len; i++) {
        reg_wr(QSPI_BASE, QSPI_DTR, tx[i]);
    }

    reg_wr(QSPI_BASE, QSPI_SSR, 0xFFFFFFFEu);           /* select slave 0   */
    reg_wr(QSPI_BASE, QSPI_CR,  CR_IDLE & ~QSPI_CR_INHIBIT);   /* go        */

    for (int i = 0; i < len; i++) {
        int spin = SPIN;
        while (reg_rd(QSPI_BASE, QSPI_SR) & QSPI_SR_RX_EMPTY) {
            if (--spin <= 0) {
                reg_wr(QSPI_BASE, QSPI_CR,  CR_IDLE);
                reg_wr(QSPI_BASE, QSPI_SSR, 0xFFFFFFFFu);
                bus_reset();
                return -1;
            }
        }
        rx[i] = (uint8_t)(reg_rd(QSPI_BASE, QSPI_DRR) & 0xFF);
    }

    reg_wr(QSPI_BASE, QSPI_CR,  CR_IDLE);               /* inhibit again    */
    reg_wr(QSPI_BASE, QSPI_SSR, 0xFFFFFFFFu);           /* deselect         */
    return 0;
}

static int rd(uint8_t reg, uint8_t *out, int n)
{
    uint8_t tx[8] = { CMD_READ, reg };
    uint8_t rx[8];

    if (n > 6) return -1;
    if (xfer(tx, rx, n + 2) < 0) return -1;

    /* The first two bytes arrive during the command; data starts at 2. */
    for (int i = 0; i < n; i++) out[i] = rx[i + 2];
    return 0;
}

static int wr(uint8_t reg, uint8_t val)
{
    uint8_t tx[3] = { CMD_WRITE, reg, val };
    uint8_t rx[3];

    return xfer(tx, rx, 3);
}

int accel_init(void)
{
    uint8_t id;

    bus_reset();
    if (rd(REG_DEVID, &id, 1) < 0) return -1;
    if (id != DEVID_AD)            return -1;

    /* Leave standby and start measuring. */
    return wr(REG_POWER, POWER_MEAS);
}

int accel_read(int8_t *x, int8_t *y, int8_t *z)
{
    uint8_t b[3];

    if (rd(REG_XDATA, b, 3) < 0) return -1;

    *x = (int8_t)b[0];
    *y = (int8_t)b[1];
    *z = (int8_t)b[2];
    return 0;
}
