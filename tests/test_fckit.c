#include "fckit/fckit.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_string(void)
{
    char bytes[12] = "abcdefghij";
    fckit_memmove(bytes + 2, bytes, 8);
    assert(fckit_memcmp(bytes, "ababcdefgh", 10) == 0);
    assert(fckit_strlen("rabbit") == 6);
    assert(fckit_strnlen("rabbit", 3) == 3);
    assert(fckit_strcmp("a", "b") < 0);
    assert(fckit_strncmp("same", "sample", 3) == 0);
    assert(fckit_strchr("abc", 'b') != NULL);
    assert(fckit_strrchr("abca", 'a')[1] == '\0');
    assert(fckit_strstr("freestanding", "stand") != NULL);
}

static void test_numeric(void)
{
    const char *end;
    uint64_t unsigned_value;
    int64_t signed_value;
    assert(fckit_parse_u64("  0xff!", 0, &unsigned_value, &end) == FCKIT_PARSE_OK);
    assert(unsigned_value == 255 && *end == '!');
    assert(fckit_parse_i64("-9223372036854775808", 10, &signed_value, &end)
        == FCKIT_PARSE_OK);
    assert(signed_value == INT64_MIN && *end == '\0');
    assert(fckit_parse_u64("18446744073709551616", 10, &unsigned_value, NULL)
        == FCKIT_PARSE_OVERFLOW);
    assert(unsigned_value == UINT64_MAX);
    assert(fckit_parse_i64("nope", 10, &signed_value, NULL) == FCKIT_PARSE_NO_DIGITS);
}

static void test_format(void)
{
    char output[64];
    int length = fckit_snprintf(
        output,
        sizeof(output),
        "%s %04d %#x %zu",
        "kit",
        -7,
        42u,
        (size_t)9);
    assert(length == 15);
    assert(fckit_strcmp(output, "kit -007 0x2a 9") == 0);
    length = fckit_snprintf(output, 5, "abcdef");
    assert(length == 6);
    assert(fckit_strcmp(output, "abcd") == 0);
    assert(fckit_snprintf(output, sizeof(output), "%.0u", 0u) == 0);
    assert(output[0] == '\0');
}

static void test_arena(void)
{
    _Alignas(64) unsigned char memory[256];
    fckit_arena arena;
    assert(fckit_arena_init(&arena, memory, sizeof(memory)));
    unsigned char *low = (unsigned char *)fckit_arena_alloc_low(&arena, 31, 16);
    unsigned char *high = (unsigned char *)fckit_arena_alloc_high(&arena, 33, 32);
    assert(low != NULL && high != NULL && low + 31 <= high);
    assert(((uintptr_t)low & 15u) == 0);
    assert(((uintptr_t)high & 31u) == 0);
    for (size_t index = 0; index < 31; ++index)
        assert(low[index] == 0);
    size_t low_mark = fckit_arena_low_mark(&arena);
    assert(fckit_arena_alloc_low(&arena, 8, 8) != NULL);
    assert(fckit_arena_rewind_low(&arena, low_mark));
    assert(!fckit_arena_rewind_low(&arena, low_mark + 1));
    assert(fckit_arena_available(&arena) > 0);
    fckit_arena_reset(&arena);
    assert(fckit_arena_available(&arena) == sizeof(memory));
}

int main(void)
{
    test_string();
    test_numeric();
    test_format();
    test_arena();
    puts("freestanding-c-kit tests passed");
    return 0;
}
