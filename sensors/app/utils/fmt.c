#include "fmt.h"

size_t fmt_str(char *buf, size_t len, const char *s) {
    while (*s) buf[len++] = *s++;
    return len;
}

size_t fmt_u32(char *buf, size_t len, uint32_t v) {
    char tmp[10];
    int  n = 0;

    if (v == 0) { buf[len++] = '0'; return len; }
    while (v) { tmp[n++] = (char)('0' + (v % 10)); v /= 10; }
    while (n) buf[len++] = tmp[--n];
    return len;
}

size_t fmt_s32(char *buf, size_t len, int32_t v)
{
    if (v < 0) { buf[len++] = '-'; v = -v; }
    return fmt_u32(buf, len, (uint32_t)v);
}
