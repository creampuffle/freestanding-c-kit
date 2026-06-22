#ifndef FCKIT_FORMAT_H
#define FCKIT_FORMAT_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*fckit_write_fn)(void *context, const char *bytes, size_t length);

int fckit_vformat(
    fckit_write_fn write,
    void *context,
    const char *format,
    va_list arguments);
int fckit_format(fckit_write_fn write, void *context, const char *format, ...);
int fckit_vsnprintf(char *output, size_t capacity, const char *format, va_list arguments);
int fckit_snprintf(char *output, size_t capacity, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
