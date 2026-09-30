/*=============================================================================
 * blink_irq -- minimal test of the timer and external interrupt paths.
 *
 *   LD0   blinks at 1 Hz from the FreeRTOS tick (CLINT timer interrupt)
 *   LD1   toggled by BTNC, through the GPIO, the AXI INTC and meip
 *
 * Needs a bitstream built with USE_INTC 1 in SoC/bd/create_bd.tcl.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "soc.h"
#include "drivers.h"
#include "intc.h"


#define BLINK_DELAY pdMS_TO_TICKS(500)
#define DEBOUNCE_TICKS pdMS_TO_TICKS(50)


static void blink_task(void *arg)
{
    (void)arg;

    /* Enable the source, then the INTC, then the core. Done in the task, not
     * in main(): the AXI GPIO latches an interrupt after reset, which would
     * otherwise fire before the scheduler has started. */
    btn_irq_clear();
    btn_irq_init();                 /* 1. GPIO                               */
    intc_init(IRQ_BTN_MASK);        /* 2. INTC                               */
    __asm volatile ("csrs mie, %0" :: "r"(MIE_MEIE));   /* 3. mie.MEIE        */

    for (;;) {
        led_toggle(LED_BLINK);
        vTaskDelay(BLINK_DELAY);
    }
}

void freertos_risc_v_application_interrupt_handler(void)
{
    static TickType_t xLastPress;
    TickType_t xNow;
    uint32_t   pending;

    /* Ask the INTC which input asserted. */
    pending = intc_pending();

    if (pending & IRQ_BTN_MASK) {
        xNow = xTaskGetTickCountFromISR();
        if ((btn_read() & BTN_CENTER) && (xNow - xLastPress) >= DEBOUNCE_TICKS) {
            xLastPress = xNow;
            led_toggle(LED_BUTTON);
        }

        /* Clear the source before acknowledging the INTC: its inputs are
         * level-sensitive. */
        btn_irq_clear();
        intc_ack(IRQ_BTN_MASK);
    }
}

int main(void)
{
    extern void freertos_risc_v_trap_handler(void);

    /* The FreeRTOS RISC-V port does not set mtvec. */
    __asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)freertos_risc_v_trap_handler));

    xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) { }
}
