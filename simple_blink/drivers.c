#include "soc.h"
#include "drivers.h"

static uint32_t led_shadow;

void led_set(uint32_t mask, int on)
{
    if (on) led_shadow |=  mask;
    else    led_shadow &= ~mask;
    reg_wr(GPIO_OUT_BASE, GPIO_DATA, led_shadow);
}

void led_toggle(uint32_t mask)
{
    led_shadow ^= mask;
    reg_wr(GPIO_OUT_BASE, GPIO_DATA, led_shadow);
}

void btn_irq_init(void)
{
    reg_wr(GPIO_IN_BASE, GPIO_IP_ISR, reg_rd(GPIO_IN_BASE, GPIO_IP_ISR));
    reg_wr(GPIO_IN_BASE, GPIO_IP_IER, GPIO_CH1_IRQ);
    reg_wr(GPIO_IN_BASE, GPIO_GIER,   GPIO_GIER_ENABLE);
}

uint32_t btn_read(void)
{
    return reg_rd(GPIO_IN_BASE, GPIO_DATA);
}

uint32_t btn_irq_pending(void)
{
    return reg_rd(GPIO_IN_BASE, GPIO_IP_ISR) & GPIO_CH1_IRQ;
}

void btn_irq_clear(void)
{
    reg_wr(GPIO_IN_BASE, GPIO_IP_ISR, GPIO_CH1_IRQ);
}
