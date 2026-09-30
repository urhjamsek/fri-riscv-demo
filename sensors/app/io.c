/*=============================================================================
 * io.c -- shared LEDs and console.
 *
 * LEDs
 *   One GPIO register holds the counter (LD15..LD12), the BTNC toggle (LD1)
 *   and the blink (LD0), written by three tasks. Each update is a short
 *   read-modify-write of a shadow copy, protected by a critical section.
 *
 * Console
 *   Two tasks print. Output is protected by a mutex, because writing a line
 *   to the 16-byte UART FIFO takes milliseconds. Lines are formatted into a
 *   local buffer first and the lock is taken with a timeout; on timeout the
 *   line is dropped. The mutex is never taken in an ISR: received characters
 *   reach tasks through a queue.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "soc.h"
#include "gpio.h"
#include "uart.h"
#include "io.h"
#include "fmt.h"

#define LOCK_MS   50

static uint32_t          led_shadow;
static SemaphoreHandle_t m_uart;
static QueueHandle_t     q_rx;

void io_init(void) {
    m_uart = xSemaphoreCreateMutex();
    q_rx   = xQueueCreate(32, sizeof(char));
    uart_hw_init();
}

/*--------------------------------------------------------------------------*/
static void led_commit(uint32_t next) {
    led_shadow = next;
    gpio_led_write(led_shadow);
}

void io_led_set(uint32_t mask, int on) {
    taskENTER_CRITICAL();
    led_commit(on ? (led_shadow | mask) : (led_shadow & ~mask));
    taskEXIT_CRITICAL();
}

void io_led_toggle(uint32_t mask) {
    taskENTER_CRITICAL();
    led_commit(led_shadow ^ mask);
    taskEXIT_CRITICAL();
}

void io_led_field(uint32_t mask, uint32_t value) {
    taskENTER_CRITICAL();
    led_commit((led_shadow & ~mask) | (value & mask));
    taskEXIT_CRITICAL();
}

/*--------------------------------------------------------------------------*/
static void tx_raw(const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while (uart_tx_full()) {
            /* wait for FIFO space */
        }
        uart_tx_byte(buf[i]);
    }
}

void io_write(const char *buf, size_t len) {
    /* Before the scheduler runs, write directly without the mutex. */
    if (m_uart == NULL || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        tx_raw(buf, len);
        return;
    }

    if (xSemaphoreTake(m_uart, pdMS_TO_TICKS(LOCK_MS)) == pdTRUE) {
        tx_raw(buf, len);
        xSemaphoreGive(m_uart);
    }
    /* else the line is dropped */
}

void io_print(const char *s) {
    size_t n = 0;
    while (s[n]) n++;
    io_write(s, n);
}

void io_line(const char *label, uint32_t value, const char *note) {
    char   buf[64];
    size_t n;

    n = fmt_str(buf, 0, label);
    n = fmt_u32(buf, n, value);
    if (note) {
        n = fmt_str(buf, n, "  (");
        n = fmt_str(buf, n, note);
        n = fmt_str(buf, n, ")");
    }
    n = fmt_str(buf, n, "\r\n");
    io_write(buf, n);
}

/*--------------------------------------------------------------------------*/
/* Task context only. Enabling the RX interrupt before the scheduler runs
 * could deliver a character before any task exists to receive it. */
void io_rx_start(void) {
    uart_rx_irq_enable();
}

void io_readline(char *buf, size_t max) {
    size_t len = 0;
    char   c;

    while (1) {
        if (xQueueReceive(q_rx, &c, portMAX_DELAY) != pdTRUE) continue;

        if (c == '\r' || c == '\n') {
            io_print("\r\n");
            buf[len] = '\0';
            return;
        }
        if (c == '\b' || c == 0x7F) {
            if (len) { len--; io_print("\b \b"); }
            continue;
        }
        if (len < max - 1 && c >= ' ') {
            buf[len++] = c;
            io_write(&c, 1);            /* echo */
        }
    }
}

/* ISR context. Drains the whole RX FIFO, since UART Lite raises its
 * interrupt only once per arrival. */
void io_rx_isr(void) {
    BaseType_t woken = pdFALSE;

    while (uart_rx_ready()) {
        char c = uart_rx_byte();
        xQueueSendFromISR(q_rx, &c, &woken);
    }

    portYIELD_FROM_ISR(woken);
}
