#ifndef FMT_H
#define FMT_H
#include <stddef.h>
#include <stdint.h>

/* Append to buf and return the new length. Used instead of printf, which
 * would pull in libc. */
size_t fmt_str(char *buf, size_t len, const char *s);
size_t fmt_u32(char *buf, size_t len, uint32_t v);

#endif
