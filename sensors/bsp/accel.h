#ifndef ACCEL_H
#define ACCEL_H
#include <stdint.h>

/* ADXL362 on the board's SPI bus.
 * 0 on success, -1 if the sensor does not answer. Never blocks forever. */
int accel_init(void);
int accel_read(int8_t *x, int8_t *y, int8_t *z);

#endif
