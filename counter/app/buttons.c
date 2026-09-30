/*=============================================================================
 * buttons.c -- button task.
 *
 * BTNC toggles LD1 directly. BTNU and BTND post +1 / -1 to q_count.
 * Waiting and debouncing are in btn_wait() (irq.c).
 *===========================================================================*/
#include "FreeRTOS.h"
#include "queue.h"
#include "soc.h"
#include "app.h"
#include "io.h"
#include "irq.h"

static void post(evt_src_t src, int8_t delta) {
    count_evt_t evt = { src, delta, 0 };
    xQueueSend(q_count, &evt, 0);
}

void t_button(void *arg) {
    (void)arg;

    while (1) {
        uint32_t pressed = btn_wait();

        if (pressed & BTN_CENTER) {
            io_led_toggle(LED_BUTTON);
            io_print("LD1 toggled  (BTNC)\r\n");
        }
        if (pressed & BTN_UP)   post(SRC_BTNU, +1);
        if (pressed & BTN_DOWN) post(SRC_BTND, -1);
    }
}
