/*=============================================================================
 * main.c -- create the queue, timer and tasks, then start the scheduler.
 *
 *   LD15..LD12   counter 0-15, changed by BTNU, BTND and the console
 *   LD1          toggled by BTNC
 *   LD0          1 Hz, from a software timer
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "soc.h"
#include "app.h"
#include "io.h"
#include "irq.h"

QueueHandle_t q_count;

static void blink_cb(TimerHandle_t t) {
    (void)t;
    io_led_toggle(LED_BLINK);
}

int main(void) {
    TimerHandle_t tmr_blink;

    io_init();
    io_print("\r\nfri-riscv demo -- RV32IMC_Zicsr + FreeRTOS\r\n");

    q_count = xQueueCreate(8, sizeof(count_evt_t));
    if (!q_count) {
        io_print("out of heap -- check configTOTAL_HEAP_SIZE\r\n");
        while (1);
    }

    tmr_blink = xTimerCreate("blink", pdMS_TO_TICKS(500), pdTRUE, NULL, blink_cb);
    if (!tmr_blink) {
        io_print("no timer -- is configUSE_TIMERS 1?\r\n");
        while (1);
    }
    xTimerStart(tmr_blink, 0);

    xTaskCreate(t_button,  "btn", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    xTaskCreate(t_counter, "cnt", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(t_console, "con", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);

    irq_init();

    vTaskStartScheduler();

    while (1);
}
