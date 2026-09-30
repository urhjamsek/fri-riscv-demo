#ifndef APP_H
#define APP_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

/* Source of a counter event, reported on the console. */
typedef enum { SRC_BTNU, SRC_BTND, SRC_UART } evt_src_t;

typedef struct {
    evt_src_t src;
    int8_t    delta;    /* +1 / -1, or 0 for an absolute set */
    uint8_t   value;    /* used when delta == 0              */
} count_evt_t;

/* Counter events. BTNU, BTND and the console post; t_counter reads. */
extern QueueHandle_t q_count;

void t_counter(void *arg);
void t_button(void *arg);
void t_console(void *arg);

#endif
