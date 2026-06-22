/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fckit/string.h"

void *fckit_memcpy(void *destination, const void *source, size_t count)
{
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;
    for (size_t index = 0; index < count; ++index)
        output[index] = input[index];
    return destination;
}

void *fckit_memmove(void *destination, const void *source, size_t count)
{
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;
    if (output < input) {
        for (size_t index = 0; index < count; ++index)
            output[index] = input[index];
    } else if (output > input) {
        while (count != 0) {
            --count;
            output[count] = input[count];
        }
    }
    return destination;
}

void *fckit_memset(void *destination, int value, size_t count)
{
    unsigned char *output = (unsigned char *)destination;
    for (size_t index = 0; index < count; ++index)
        output[index] = (unsigned char)value;
    return destination;
}

int fckit_memcmp(const void *left, const void *right, size_t count)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    for (size_t index = 0; index < count; ++index) {
        if (a[index] != b[index])
            return a[index] < b[index] ? -1 : 1;
    }
    return 0;
}

size_t fckit_strlen(const char *text)
{
    const char *cursor = text;
    while (*cursor != '\0')
        ++cursor;
    return (size_t)(cursor - text);
}

size_t fckit_strnlen(const char *text, size_t limit)
{
    size_t length = 0;
    while (length < limit && text[length] != '\0')
        ++length;
    return length;
}

int fckit_strcmp(const char *left, const char *right)
{
    while (*left != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return (unsigned char)*left < (unsigned char)*right
        ? -1
        : (unsigned char)*left != (unsigned char)*right;
}

int fckit_strncmp(const char *left, const char *right, size_t count)
{
    for (size_t index = 0; index < count; ++index) {
        unsigned char a = (unsigned char)left[index];
        unsigned char b = (unsigned char)right[index];
        if (a != b)
            return a < b ? -1 : 1;
        if (a == 0)
            return 0;
    }
    return 0;
}

const char *fckit_strchr(const char *text, int character)
{
    char wanted = (char)character;
    for (;;) {
        if (*text == wanted)
            return text;
        if (*text == '\0')
            return NULL;
        ++text;
    }
}

const char *fckit_strrchr(const char *text, int character)
{
    const char *found = NULL;
    char wanted = (char)character;
    do {
        if (*text == wanted)
            found = text;
    } while (*text++ != '\0');
    return found;
}

const char *fckit_strstr(const char *text, const char *needle)
{
    size_t needle_length = fckit_strlen(needle);
    if (needle_length == 0)
        return text;
    while (*text != '\0') {
        if (fckit_strncmp(text, needle, needle_length) == 0)
            return text;
        ++text;
    }
    return NULL;
}
