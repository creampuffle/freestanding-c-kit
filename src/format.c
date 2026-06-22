/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fckit/format.h"

#include "fckit/string.h"

#include <stdint.h>

typedef struct format_state {
    fckit_write_fn write;
    void *context;
    size_t count;
    int failed;
} format_state;

typedef struct buffer_state {
    char *output;
    size_t capacity;
    size_t used;
} buffer_state;

static void emit(format_state *state, const char *bytes, size_t length)
{
    if (state->failed || length == 0)
        return;
    if (length > (size_t)INT32_MAX - state->count) {
        state->failed = 1;
        return;
    }
    if (state->write != NULL && state->write(state->context, bytes, length) != 0) {
        state->failed = 1;
        return;
    }
    state->count += length;
}

static void emit_repeat(format_state *state, char character, int count)
{
    char block[16];
    fckit_memset(block, character, sizeof(block));
    while (count > 0) {
        size_t amount = count > (int)sizeof(block) ? sizeof(block) : (size_t)count;
        emit(state, block, amount);
        count -= (int)amount;
    }
}

static size_t unsigned_text(char *output, uint64_t value, unsigned base, int upper)
{
    static const char lower_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char upper_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = upper ? upper_digits : lower_digits;
    char reverse[65];
    size_t length = 0;
    do {
        reverse[length++] = digits[value % base];
        value /= base;
    } while (value != 0);
    for (size_t index = 0; index < length; ++index)
        output[index] = reverse[length - index - 1u];
    return length;
}

static void emit_field(
    format_state *state,
    const char *text,
    size_t length,
    char sign,
    const char *prefix,
    size_t prefix_length,
    int width,
    int precision,
    int left,
    int zero)
{
    int zeroes = precision > (int)length ? precision - (int)length : 0;
    int content = (int)length + zeroes + (sign != '\0') + (int)prefix_length;
    int padding = width > content ? width - content : 0;
    if (!left && !zero)
        emit_repeat(state, ' ', padding);
    if (sign != '\0')
        emit(state, &sign, 1);
    emit(state, prefix, prefix_length);
    if (!left && zero)
        emit_repeat(state, '0', padding);
    emit_repeat(state, '0', zeroes);
    emit(state, text, length);
    if (left)
        emit_repeat(state, ' ', padding);
}

int fckit_vformat(
    fckit_write_fn write,
    void *context,
    const char *format,
    va_list arguments)
{
    format_state state = {write, context, 0, 0};
    if (format == NULL)
        return -1;

    while (*format != '\0') {
        const char *literal = format;
        while (*format != '\0' && *format != '%')
            ++format;
        emit(&state, literal, (size_t)(format - literal));
        if (*format == '\0')
            break;
        ++format;
        if (*format == '%') {
            emit(&state, format, 1);
            ++format;
            continue;
        }

        int left = 0;
        int zero = 0;
        int plus = 0;
        int space = 0;
        int alternate = 0;
        int width = 0;
        int precision = -1;
        int long_count = 0;
        for (;;) {
            if (*format == '-') left = 1;
            else if (*format == '0') zero = 1;
            else if (*format == '+') plus = 1;
            else if (*format == ' ') space = 1;
            else if (*format == '#') alternate = 1;
            else break;
            ++format;
        }
        if (*format == '*') {
            width = va_arg(arguments, int);
            ++format;
            if (width < 0) {
                left = 1;
                if (width == INT32_MIN)
                    return -1;
                width = -width;
            }
        } else {
            while (*format >= '0' && *format <= '9') {
                int digit = *format - '0';
                if (width > (INT32_MAX - digit) / 10)
                    return -1;
                width = width * 10 + digit;
                ++format;
            }
        }
        if (*format == '.') {
            ++format;
            precision = 0;
            if (*format == '*') {
                precision = va_arg(arguments, int);
                ++format;
                if (precision < 0)
                    precision = -1;
            } else {
                while (*format >= '0' && *format <= '9') {
                    int digit = *format - '0';
                    if (precision > (INT32_MAX - digit) / 10)
                        return -1;
                    precision = precision * 10 + digit;
                    ++format;
                }
            }
        }
        if (*format == 'l') {
            ++format;
            long_count = 1;
            if (*format == 'l') {
                ++format;
                long_count = 2;
            }
        } else if (*format == 'z') {
            ++format;
            long_count = 3;
        }
        if (left || precision >= 0)
            zero = 0;

        char conversion = *format;
        if (conversion == '\0')
            return -1;
        ++format;
        if (conversion == 'c') {
            char character = (char)va_arg(arguments, int);
            emit_field(&state, &character, 1, '\0', NULL, 0, width, -1, left, 0);
        } else if (conversion == 's') {
            const char *text = va_arg(arguments, const char *);
            size_t length;
            if (text == NULL)
                text = "(null)";
            length = precision < 0
                ? fckit_strlen(text)
                : fckit_strnlen(text, (size_t)precision);
            emit_field(&state, text, length, '\0', NULL, 0, width, -1, left, 0);
        } else if (conversion == 'd' || conversion == 'i') {
            int64_t signed_value;
            uint64_t magnitude;
            char text[65];
            char sign = '\0';
            if (long_count == 2) signed_value = va_arg(arguments, long long);
            else if (long_count == 1) signed_value = va_arg(arguments, long);
            else if (long_count == 3) signed_value = (int64_t)va_arg(arguments, ptrdiff_t);
            else signed_value = va_arg(arguments, int);
            if (signed_value < 0) {
                sign = '-';
                magnitude = (uint64_t)(-(signed_value + 1)) + 1u;
            } else {
                magnitude = (uint64_t)signed_value;
                sign = plus ? '+' : (space ? ' ' : '\0');
            }
            size_t length = precision == 0 && magnitude == 0
                ? 0
                : unsigned_text(text, magnitude, 10, 0);
            emit_field(&state, text, length, sign, NULL, 0, width,
                precision, left, zero);
        } else if (conversion == 'u' || conversion == 'x' || conversion == 'X'
            || conversion == 'o' || conversion == 'p') {
            uint64_t value;
            unsigned base = conversion == 'o' ? 8u
                : (conversion == 'u' ? 10u : 16u);
            int upper = conversion == 'X';
            const char *prefix = NULL;
            size_t prefix_length = 0;
            char text[65];
            if (conversion == 'p') {
                value = (uint64_t)(uintptr_t)va_arg(arguments, void *);
                prefix = "0x";
                prefix_length = 2;
            } else if (long_count == 2) value = va_arg(arguments, unsigned long long);
            else if (long_count == 1) value = va_arg(arguments, unsigned long);
            else if (long_count == 3) value = (uint64_t)va_arg(arguments, size_t);
            else value = va_arg(arguments, unsigned int);
            if (alternate && value != 0) {
                if (base == 16) {
                    prefix = upper ? "0X" : "0x";
                    prefix_length = 2;
                } else if (base == 8) {
                    prefix = "0";
                    prefix_length = 1;
                }
            }
            size_t length = precision == 0 && value == 0 && conversion != 'p'
                ? 0
                : unsigned_text(text, value, base, upper);
            emit_field(&state, text, length, '\0', prefix, prefix_length,
                width, precision, left, zero);
        } else {
            return -1;
        }
    }
    return state.failed ? -1 : (int)state.count;
}

int fckit_format(fckit_write_fn write, void *context, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = fckit_vformat(write, context, format, arguments);
    va_end(arguments);
    return result;
}

static int buffer_write(void *context, const char *bytes, size_t length)
{
    buffer_state *buffer = (buffer_state *)context;
    if (buffer->capacity != 0 && buffer->used < buffer->capacity - 1u) {
        size_t available = buffer->capacity - buffer->used - 1u;
        size_t amount = length < available ? length : available;
        fckit_memcpy(buffer->output + buffer->used, bytes, amount);
        buffer->used += amount;
    }
    return 0;
}

int fckit_vsnprintf(char *output, size_t capacity, const char *format, va_list arguments)
{
    buffer_state buffer = {output, capacity, 0};
    int result;
    if (capacity != 0 && output == NULL)
        return -1;
    result = fckit_vformat(buffer_write, &buffer, format, arguments);
    if (capacity != 0)
        output[buffer.used] = '\0';
    return result;
}

int fckit_snprintf(char *output, size_t capacity, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = fckit_vsnprintf(output, capacity, format, arguments);
    va_end(arguments);
    return result;
}
