/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FCKIT_NUMERIC_H
#define FCKIT_NUMERIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum fckit_parse_status {
    FCKIT_PARSE_OK = 0,
    FCKIT_PARSE_NO_DIGITS = 1,
    FCKIT_PARSE_OVERFLOW = 2,
    FCKIT_PARSE_INVALID_BASE = 3
} fckit_parse_status;

fckit_parse_status fckit_parse_u64(
    const char *text,
    unsigned base,
    uint64_t *value,
    const char **end);
fckit_parse_status fckit_parse_i64(
    const char *text,
    unsigned base,
    int64_t *value,
    const char **end);

#ifdef __cplusplus
}
#endif

#endif
