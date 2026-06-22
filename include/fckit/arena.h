/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: GPL-3.0-only
 * Additional terms apply; see ADDITIONAL_TERMS.
 */

#ifndef FCKIT_ARENA_H
#define FCKIT_ARENA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct fckit_arena {
    unsigned char *memory;
    size_t capacity;
    size_t low;
    size_t high;
} fckit_arena;

int fckit_arena_init(fckit_arena *arena, void *memory, size_t capacity);
void fckit_arena_reset(fckit_arena *arena);
void *fckit_arena_alloc_low(fckit_arena *arena, size_t size, size_t alignment);
void *fckit_arena_alloc_high(fckit_arena *arena, size_t size, size_t alignment);
size_t fckit_arena_low_mark(const fckit_arena *arena);
size_t fckit_arena_high_mark(const fckit_arena *arena);
int fckit_arena_rewind_low(fckit_arena *arena, size_t mark);
int fckit_arena_rewind_high(fckit_arena *arena, size_t mark);
size_t fckit_arena_available(const fckit_arena *arena);

#ifdef __cplusplus
}
#endif

#endif
