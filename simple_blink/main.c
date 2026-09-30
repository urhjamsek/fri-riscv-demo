#include "FreeRTOS.h"
#include "task.h"
#include "soc.h"
#include "drivers.h"

#ifdef SIM_FAST
#  define BLINK_DELAY    ( ( TickType_t ) 2 )
#  define DEBOUNCE_TICKS ( ( TickType_t ) 2 )
#else
#  define BLINK_DELAY    pdMS_TO_TICKS( 500 )
#  define DEBOUNCE_TICKS pdMS_TO_TICKS( 50 )
#endif

static void blink_task(void *arg)
{
    (void)arg;

    btn_irq_clear();
    btn_irq_init();
    __asm volatile ("csrs mie, %0" :: "r"(MIE_MEIE));

    for (;;) {
        led_toggle(LED_BLINK);
        vTaskDelay(BLINK_DELAY);
    }
}

void freertos_risc_v_application_interrupt_handler(void)
{
    static TickType_t xLastPress;
    TickType_t xNow;

    if (!btn_irq_pending()) {
        return;
    }

    xNow = xTaskGetTickCountFromISR();
    if ((btn_read() & BTN_CENTER) && (xNow - xLastPress) >= DEBOUNCE_TICKS) {
        xLastPress = xNow;
        led_toggle(LED_BUTTON);
    }

    btn_irq_clear();
}

int main(void)
{
    extern void freertos_risc_v_trap_handler(void);

    __asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)freertos_risc_v_trap_handler));

    xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) { }
}
