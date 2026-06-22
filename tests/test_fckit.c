#include "fckit/fckit.h"

#include <stdint.h>
#include <stdio.h>

#define CHECK(expression) \
    do { \
        if (!(expression)) { \
            fprintf(stderr, "check failed at %s:%d: %s\n", \
                __FILE__, __LINE__, #expression); \
            return 0; \
        } \
    } while (0)

static int test_string(void)
{
    char bytes[12] = "abcdefghij";
    fckit_memmove(bytes + 2, bytes, 8);
    CHECK(fckit_memcmp(bytes, "ababcdefgh", 10) == 0);
    CHECK(fckit_strlen("rabbit") == 6);
    CHECK(fckit_strnlen("rabbit", 3) == 3);
    CHECK(fckit_strcmp("a", "b") < 0);
    CHECK(fckit_strncmp("same", "sample", 3) == 0);
    CHECK(fckit_strchr("abc", 'b') != NULL);
    CHECK(fckit_strrchr("abca", 'a')[1] == '\0');
    CHECK(fckit_strstr("freestanding", "stand") != NULL);
    return 1;
}

static int test_numeric(void)
{
    const char *end;
    uint64_t unsigned_value;
    int64_t signed_value;
    CHECK(fckit_parse_u64("  0xff!", 0, &unsigned_value, &end) == FCKIT_PARSE_OK);
    CHECK(unsigned_value == 255 && *end == '!');
    CHECK(fckit_parse_i64("-9223372036854775808", 10, &signed_value, &end)
        == FCKIT_PARSE_OK);
    CHECK(signed_value == INT64_MIN && *end == '\0');
    CHECK(fckit_parse_u64("18446744073709551616", 10, &unsigned_value, NULL)
        == FCKIT_PARSE_OVERFLOW);
    CHECK(unsigned_value == UINT64_MAX);
    CHECK(fckit_parse_i64("nope", 10, &signed_value, NULL) == FCKIT_PARSE_NO_DIGITS);
    return 1;
}

static int test_format(void)
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
    CHECK(length == 15);
    CHECK(fckit_strcmp(output, "kit -007 0x2a 9") == 0);
    length = fckit_snprintf(output, 5, "abcdef");
    CHECK(length == 6);
    CHECK(fckit_strcmp(output, "abcd") == 0);
    CHECK(fckit_snprintf(output, sizeof(output), "%.0u", 0u) == 0);
    CHECK(output[0] == '\0');
    return 1;
}

static int test_arena(void)
{
    _Alignas(64) unsigned char memory[256];
    fckit_arena arena;
    CHECK(fckit_arena_init(&arena, memory, sizeof(memory)));
    unsigned char *low = (unsigned char *)fckit_arena_alloc_low(&arena, 31, 16);
    unsigned char *high = (unsigned char *)fckit_arena_alloc_high(&arena, 33, 32);
    CHECK(low != NULL && high != NULL && low + 31 <= high);
    CHECK(((uintptr_t)low & 15u) == 0);
    CHECK(((uintptr_t)high & 31u) == 0);
    for (size_t index = 0; index < 31; ++index)
        CHECK(low[index] == 0);
    size_t low_mark = fckit_arena_low_mark(&arena);
    CHECK(fckit_arena_alloc_low(&arena, 8, 8) != NULL);
    CHECK(fckit_arena_rewind_low(&arena, low_mark));
    CHECK(!fckit_arena_rewind_low(&arena, low_mark + 1));
    CHECK(fckit_arena_available(&arena) > 0);
    fckit_arena_reset(&arena);
    CHECK(fckit_arena_available(&arena) == sizeof(memory));
    return 1;
}

int main(void)
{
    if (!test_string() || !test_numeric() || !test_format() || !test_arena())
        return 1;
    puts("freestanding-c-kit tests passed");
    return 0;
}
