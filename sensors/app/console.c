/*=============================================================================
 * console.c -- console task: parses commands typed over the UART.
 *
 * Counter commands post the same count_evt_t to q_count as the buttons.
 * Echo, backspace and buffering are in io_readline() (io.c).
 *===========================================================================*/
#include "FreeRTOS.h"
#include "queue.h"
#include "app.h"
#include "io.h"
#include "fmt.h"
#include "temp.h"
#include "accel.h"

#define LINE_MAX  32

static const char *HELP =
    "\r\ncommands\r\n"
    "  inc        increment the counter\r\n"
    "  dec        decrement\r\n"
    "  set <0-15> absolute value\r\n"
    "  temp       read the ADT7420\r\n"
    "  accel      read the ADXL362\r\n"
    "  help\r\n";

static int is(const char *line, const char *word) {
    while (*word) { if (*line++ != *word++) return 0; }
    return (*line == '\0' || *line == ' ');
}

/* -1 when there is no number. */
static int number(const char *s) {
    int v = -1;
    while (*s == ' ') s++;
    while (*s >= '0' && *s <= '9') {
        if (v < 0) v = 0;
        v = v * 10 + (*s++ - '0');
        if (v > 255) return -1;
    }
    return v;
}

static void post(int8_t delta, uint8_t value) {
    count_evt_t evt = { SRC_UART, delta, value };
    xQueueSend(q_count, &evt, 0);
}

/* The sensors are used only by this task, so they need no locking. */
static void show_temp(void)
{
    char   buf[32];
    size_t n;
    int    t;

    if (temp_read(&t) < 0) { io_print("adt7420: no response\r\n"); return; }

    n = fmt_str(buf, 0, "temp = ");
    n = fmt_s32(buf, n, t / 10);
    n = fmt_str(buf, n, ".");
    n = fmt_u32(buf, n, (uint32_t)(t < 0 ? -t : t) % 10);
    n = fmt_str(buf, n, " C\r\n");
    io_write(buf, n);
}

static void show_accel(void)
{
    char   buf[48];
    size_t n;
    int8_t x, y, z;

    if (accel_read(&x, &y, &z) < 0) { io_print("adxl362: no response\r\n"); return; }

    n = fmt_str(buf, 0, "accel x=");
    n = fmt_s32(buf, n, x);
    n = fmt_str(buf, n, " y=");
    n = fmt_s32(buf, n, y);
    n = fmt_str(buf, n, " z=");
    n = fmt_s32(buf, n, z);
    n = fmt_str(buf, n, "\r\n");
    io_write(buf, n);
}

static void run(const char *line) {
    int v;

    if (line[0] == '\0')      return;
    else if (is(line, "inc" )) post(+1, 0);
    else if (is(line, "dec" )) post(-1, 0);
    else if (is(line, "temp")) show_temp();
    else if (is(line, "accel")) show_accel();
    else if (is(line, "help")) io_print(HELP);
    else if (is(line, "set" )) {
        v = number(line + 3);
        if (v < 0 || v > 15) io_print("set needs a value 0-15\r\n");
        else                 post(0, (uint8_t)v);
    }
    else io_print("unknown command -- try help\r\n");
}

void t_console(void *arg) {
    char line[LINE_MAX];

    (void)arg;

    io_rx_start();          /* enable receive, from task context */
    io_print(HELP);

    while (1) {
        io_print("> ");
        io_readline(line, sizeof line);
        run(line);
    }
}
