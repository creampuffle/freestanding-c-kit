#include "fckit/numeric.h"

#include <limits.h>

static int digit_value(char character)
{
    if (character >= '0' && character <= '9')
        return character - '0';
    if (character >= 'a' && character <= 'z')
        return character - 'a' + 10;
    if (character >= 'A' && character <= 'Z')
        return character - 'A' + 10;
    return -1;
}

static int is_space(char character)
{
    return character == ' ' || (character >= '\t' && character <= '\r');
}

fckit_parse_status fckit_parse_u64(
    const char *text,
    unsigned base,
    uint64_t *value,
    const char **end)
{
    const char *cursor = text;
    uint64_t result = 0;
    int overflow = 0;
    int any = 0;

    if (value == NULL || (base != 0 && (base < 2 || base > 36))) {
        if (end != NULL)
            *end = text;
        return FCKIT_PARSE_INVALID_BASE;
    }
    while (is_space(*cursor))
        ++cursor;
    if (*cursor == '+')
        ++cursor;
    if ((base == 0 || base == 16) && cursor[0] == '0'
        && (cursor[1] == 'x' || cursor[1] == 'X')) {
        base = 16;
        cursor += 2;
    } else if (base == 0) {
        base = cursor[0] == '0' ? 8u : 10u;
    }
    for (;;) {
        int digit = digit_value(*cursor);
        if (digit < 0 || (unsigned)digit >= base)
            break;
        any = 1;
        if (result > (UINT64_MAX - (uint64_t)digit) / (uint64_t)base) {
            overflow = 1;
            result = UINT64_MAX;
        } else if (!overflow) {
            result = result * (uint64_t)base + (uint64_t)digit;
        }
        ++cursor;
    }
    if (end != NULL)
        *end = any ? cursor : text;
    *value = result;
    if (!any)
        return FCKIT_PARSE_NO_DIGITS;
    return overflow ? FCKIT_PARSE_OVERFLOW : FCKIT_PARSE_OK;
}

fckit_parse_status fckit_parse_i64(
    const char *text,
    unsigned base,
    int64_t *value,
    const char **end)
{
    const char *cursor = text;
    const char *digits_end = text;
    uint64_t magnitude = 0;
    fckit_parse_status status;
    int negative = 0;

    if (value == NULL) {
        if (end != NULL)
            *end = text;
        return FCKIT_PARSE_INVALID_BASE;
    }
    while (is_space(*cursor))
        ++cursor;
    if (*cursor == '-' || *cursor == '+') {
        negative = *cursor == '-';
        ++cursor;
    }
    status = fckit_parse_u64(cursor, base, &magnitude, &digits_end);
    if (status == FCKIT_PARSE_NO_DIGITS || status == FCKIT_PARSE_INVALID_BASE) {
        if (end != NULL)
            *end = text;
        *value = 0;
        return status;
    }
    if (end != NULL)
        *end = digits_end;
    if (negative) {
        uint64_t limit = (uint64_t)INT64_MAX + 1u;
        if (status == FCKIT_PARSE_OVERFLOW || magnitude > limit) {
            *value = INT64_MIN;
            return FCKIT_PARSE_OVERFLOW;
        }
        *value = magnitude == limit ? INT64_MIN : -(int64_t)magnitude;
    } else {
        if (status == FCKIT_PARSE_OVERFLOW || magnitude > (uint64_t)INT64_MAX) {
            *value = INT64_MAX;
            return FCKIT_PARSE_OVERFLOW;
        }
        *value = (int64_t)magnitude;
    }
    return FCKIT_PARSE_OK;
}
