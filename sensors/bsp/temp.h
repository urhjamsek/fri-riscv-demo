#ifndef TEMP_H
#define TEMP_H

/* ADT7420 on the board's I2C bus.
 * Returns 0 on success and writes tenths of a degree C (234 = 23.4 C).
 * Returns -1 if the sensor does not answer. Never blocks forever. */
int temp_init(void);
int temp_read(int *tenths);

#endif
