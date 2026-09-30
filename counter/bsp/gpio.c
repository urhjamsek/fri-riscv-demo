#include "soc.h"
#include "gpio.h"

void gpio_led_write(uint32_t value)
{
    reg_wr(GPIO_OUT_BASE, GPIO_DATA, value);
}

uint32_t gpio_btn_read(void)
{
    return reg_rd(GPIO_IN_BASE, GPIO_DATA);
}

void gpio_btn_irq_init(void)
{
    /* Clear anything pending (see gpio_btn_irq_clear). */
    reg_wr(GPIO_IN_BASE, GPIO_IP_ISR, reg_rd(GPIO_IN_BASE, GPIO_IP_ISR));
    reg_wr(GPIO_IN_BASE, GPIO_IP_IER, GPIO_CH1_IRQ);
    reg_wr(GPIO_IN_BASE, GPIO_GIER,   GPIO_GIER_ENABLE);
}

void gpio_btn_irq_enable(int on)
{
    reg_wr(GPIO_IN_BASE, GPIO_IP_IER, on ? GPIO_CH1_IRQ : 0);
}

/* IP_ISR is toggle-on-write (PG144): writing 1 to a bit that is already 0
 * sets it. Only bits that are currently set are written back. */
void gpio_btn_irq_clear(void)
{
    uint32_t pending = reg_rd(GPIO_IN_BASE, GPIO_IP_ISR) & GPIO_CH1_IRQ;

    if (pending) {
        reg_wr(GPIO_IN_BASE, GPIO_IP_ISR, pending);
    }
}
