#ifndef IRQ_H
#define IRQ_H
#include <stdint.h>

/* irq_init()  install the trap vector, switch the controller on, let the core
 *             listen. Once, from main(), before the scheduler.
 *
 * btn_wait()  block until a button is pressed AND has stopped bouncing, then
 *             return which buttons are down (BTN_* from soc.h). Task only.
 */
void     irq_init(void);
uint32_t btn_wait(void);

#endif
