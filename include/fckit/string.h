/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: GPL-3.0-only
 * Additional terms apply; see ADDITIONAL_TERMS.
 */

#ifndef FCKIT_STRING_H
#define FCKIT_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
void *fckit_memcpy(void *destination, const void *source, size_t count);
void *fckit_memmove(void *destination, const void *source, size_t count);
void *fckit_memset(void *destination, int value, size_t count);
int fckit_memcmp(const void *left, const void *right, size_t count);
size_t fckit_strlen(const char *text);
size_t fckit_strnlen(const char *text, size_t limit);
int fckit_strcmp(const char *left, const char *right);
int fckit_strncmp(const char *left, const char *right, size_t count);
const char *fckit_strchr(const char *text, int character);
const char *fckit_strrchr(const char *text, int character);
const char *fckit_strstr(const char *text, const char *needle);

#ifdef __cplusplus
}
#endif

#endif
