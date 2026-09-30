#ifndef INTC_H
#define INTC_H
#include <stdint.h>

/* Enable the INTC and the given inputs (IRQ_*_MASK from soc.h). */
void     intc_init(uint32_t mask);

/* Which enabled inputs are asserting right now. */
uint32_t intc_pending(void);

/* Acknowledge inputs. Clear them at the peripheral first. */
void     intc_ack(uint32_t mask);

#endif
