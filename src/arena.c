/*
 * Copyright (C) 2026 creampuffle
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fckit/arena.h"

#include "fckit/string.h"

#include <stdint.h>

static int valid_alignment(size_t alignment)
{
    return alignment != 0 && (alignment & (alignment - 1u)) == 0;
}

static int valid_arena(const fckit_arena *arena)
{
    return arena != NULL
        && !(arena->memory == NULL && arena->capacity != 0)
        && arena->high <= arena->capacity
        && arena->low <= arena->capacity - arena->high;
}

static int align_up_address(uintptr_t value, size_t alignment, uintptr_t *result)
{
    uintptr_t mask = (uintptr_t)alignment - 1u;
    if (value > UINTPTR_MAX - mask)
        return 0;
    *result = (value + mask) & ~mask;
    return 1;
}

int fckit_arena_init(fckit_arena *arena, void *memory, size_t capacity)
{
    if (arena == NULL || (memory == NULL && capacity != 0))
        return 0;
    arena->memory = (unsigned char *)memory;
    arena->capacity = capacity;
    arena->low = 0;
    arena->high = 0;
    return 1;
}

void fckit_arena_reset(fckit_arena *arena)
{
    if (arena == NULL)
        return;
    if (arena->low != 0)
        fckit_memset(arena->memory, 0, arena->low);
    if (arena->high != 0)
        fckit_memset(arena->memory + arena->capacity - arena->high, 0, arena->high);
    arena->low = 0;
    arena->high = 0;
}

void *fckit_arena_alloc_low(fckit_arena *arena, size_t size, size_t alignment)
{
    uintptr_t base;
    uintptr_t aligned;
    size_t start;
    size_t end;
    if (!valid_arena(arena) || !valid_alignment(alignment))
        return NULL;
    base = (uintptr_t)arena->memory;
    if (base > UINTPTR_MAX - arena->low
        || !align_up_address(base + arena->low, alignment, &aligned)
        || aligned < base)
        return NULL;
    start = (size_t)(aligned - base);
    if (size > arena->capacity - arena->high
        || start > arena->capacity - arena->high - size)
        return NULL;
    end = start + size;
    arena->low = end;
    fckit_memset(arena->memory + start, 0, size);
    return arena->memory + start;
}

void *fckit_arena_alloc_high(fckit_arena *arena, size_t size, size_t alignment)
{
    uintptr_t base;
    uintptr_t unaligned;
    uintptr_t aligned;
    size_t start;
    if (!valid_arena(arena) || !valid_alignment(alignment)
        || size > arena->capacity - arena->low - arena->high)
        return NULL;
    base = (uintptr_t)arena->memory;
    if (base > UINTPTR_MAX - (arena->capacity - arena->high - size))
        return NULL;
    unaligned = base + arena->capacity - arena->high - size;
    aligned = unaligned & ~((uintptr_t)alignment - 1u);
    if (aligned < base)
        return NULL;
    start = (size_t)(aligned - base);
    if (start < arena->low)
        return NULL;
    arena->high = arena->capacity - start;
    fckit_memset(arena->memory + start, 0, size);
    return arena->memory + start;
}

size_t fckit_arena_low_mark(const fckit_arena *arena)
{
    return arena == NULL ? 0 : arena->low;
}

size_t fckit_arena_high_mark(const fckit_arena *arena)
{
    return arena == NULL ? 0 : arena->high;
}

int fckit_arena_rewind_low(fckit_arena *arena, size_t mark)
{
    if (!valid_arena(arena) || mark > arena->low)
        return 0;
    if (arena->low != mark)
        fckit_memset(arena->memory + mark, 0, arena->low - mark);
    arena->low = mark;
    return 1;
}

int fckit_arena_rewind_high(fckit_arena *arena, size_t mark)
{
    if (!valid_arena(arena) || mark > arena->high)
        return 0;
    if (arena->high != mark) {
        fckit_memset(
            arena->memory + arena->capacity - arena->high,
            0,
            arena->high - mark);
    }
    arena->high = mark;
    return 1;
}

size_t fckit_arena_available(const fckit_arena *arena)
{
    if (!valid_arena(arena))
        return 0;
    return arena->capacity - arena->low - arena->high;
}
