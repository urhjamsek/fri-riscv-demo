#ifndef IO_H
#define IO_H
#include <stddef.h>
#include <stdint.h>

/* Shared LEDs and console. See io.c. */

/* --- LEDs, each update in a critical section ------------------------------ */
void io_led_set(uint32_t mask, int on);
void io_led_toggle(uint32_t mask);
void io_led_field(uint32_t mask, uint32_t value);   /* value pre-shifted */

/* --- Console, output behind a mutex ------------------------------------- */
void io_print(const char *s);
void io_write(const char *buf, size_t len);
void io_line(const char *label, uint32_t value, const char *note);

/* Block until a whole line is typed; newline stripped, echo and backspace
 * handled. Task context only. */
void io_readline(char *buf, size_t max);

/* Create the mutex and the receive queue. Call once, before the scheduler. */
void io_init(void);

/* Start accepting characters. Task context only. */
void io_rx_start(void);

/* ISR context only: move everything in the RX FIFO into the queue. */
void io_rx_isr(void);

#endif
