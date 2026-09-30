/*=============================================================================
 * irq.c -- external interrupt handler and button debounce.
 *
 * Button path:  GPIO -> INTC -> meip -> handler -> q_btn -> btn_wait()
 *
 * The handler only records and acknowledges; all waiting happens in tasks.
 *
 * Notes
 *   - The FreeRTOS RISC-V port does not set mtvec; irq_init() does.
 *   - The GPIO interrupt is armed on the first call to btn_wait(), in task
 *     context. The AXI GPIO latches an interrupt after reset, which would
 *     otherwise fire before the scheduler has started a task.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "soc.h"
#include "gpio.h"
#include "intc.h"
#include "io.h"
#include "irq.h"

#define DEBOUNCE_MS   20

/* Button snapshots from the handler to btn_wait(). */
static QueueHandle_t q_btn;

void irq_init(void) {
    extern void freertos_risc_v_trap_handler(void);

    q_btn = xQueueCreate(4, sizeof(uint32_t));

    __asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)freertos_risc_v_trap_handler));

    /* Safe to enable now: the peripherals' own interrupts are still off and
     * are enabled later from tasks. */
    intc_init(IRQ_BTN_MASK | IRQ_UART_MASK);
    __asm volatile ("csrs mie, %0" :: "r"(MIE_MEIE));
}

void freertos_risc_v_application_interrupt_handler(void) {
    uint32_t pending;

    /* Loop until nothing is pending: another source may assert while one is
     * being serviced. */
    while ((pending = intc_pending()) != 0) {

        if (pending & IRQ_UART_MASK) {
            io_rx_isr();                /* draining the FIFO clears the source */
            intc_ack(IRQ_UART_MASK);
        }

        if (pending & IRQ_BTN_MASK) {
            BaseType_t woken    = pdFALSE;
            uint32_t   snapshot = gpio_btn_read();

            /* Mask the GPIO interrupt until btn_wait() re-enables it, so a
             * bouncing contact produces one event. */
            gpio_btn_irq_enable(0);
            gpio_btn_irq_clear();
            xQueueSendFromISR(q_btn, &snapshot, &woken);

            /* Clear the source before acknowledging the INTC: its inputs are
             * level-sensitive. */
            intc_ack(IRQ_BTN_MASK);
            portYIELD_FROM_ISR(woken);
        }
    }
}

uint32_t btn_wait(void) {
    static int armed = 0;
    uint32_t   snapshot;

    if (!armed) {                       /* first call, see header */
        gpio_btn_irq_clear();
        gpio_btn_irq_init();
        armed = 1;
    }

    while (1) {
        if (xQueueReceive(q_btn, &snapshot, portMAX_DELAY) != pdTRUE) continue;

        /* Wait out the bounce, then read the current button levels. */
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
        snapshot = gpio_btn_read() & BTN_USED;

        gpio_btn_irq_clear();
        gpio_btn_irq_enable(1);

        if (snapshot) return snapshot;
    }
}
