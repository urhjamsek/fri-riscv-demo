#ifndef DRIVERS_H
#define DRIVERS_H
#include <stdint.h>

void     led_set(uint32_t mask, int on);
void     led_toggle(uint32_t mask);

void     btn_irq_init(void);
uint32_t btn_read(void);
uint32_t btn_irq_pending(void);
void     btn_irq_clear(void);

#endif
