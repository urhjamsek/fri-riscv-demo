#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>

/* AXI GPIO, register level. No state and no locking. */

void     gpio_led_write(uint32_t value);   /* all 16 LEDs at once */
uint32_t gpio_btn_read(void);              /* raw pin levels      */

void     gpio_btn_irq_init(void);          /* interrupt on change, channel 1 */
void     gpio_btn_irq_enable(int on);
void     gpio_btn_irq_clear(void);

#endif
