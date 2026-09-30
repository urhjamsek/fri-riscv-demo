/*=============================================================================
 * counter.c -- counter task, the only reader of q_count.
 *
 * Owns the counter value and LD15..LD12, so no locking is needed.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "queue.h"
#include "soc.h"
#include "app.h"
#include "io.h"

static const char *src_name(evt_src_t s) {
    switch (s) {
        case SRC_BTNU: return "BTNU";
        case SRC_BTND: return "BTND";
        default:       return "UART";
    }
}

static void show(uint8_t value) {
    io_led_field(LED_COUNT_MASK, (uint32_t)value << LED_COUNT_SHIFT);
}

void t_counter(void *arg) {
    count_evt_t evt;
    uint8_t     value = 0;

    (void)arg;
    show(value);

    while (1) {
        if (xQueueReceive(q_count, &evt, portMAX_DELAY) != pdTRUE) continue;

        if (evt.delta == 0) value = evt.value & 0xF;
        else                value = (uint8_t)((value + evt.delta) & 0xF);  /* wraps */

        show(value);
        io_line("counter = ", value, src_name(evt.src));
    }
}
