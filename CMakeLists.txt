cmake_minimum_required(VERSION 3.16)
project(freestanding_c_kit VERSION 0.1.0 LANGUAGES C)

option(FCKIT_BUILD_TESTS "Build the host-side tests" ON)

add_library(fckit STATIC
    src/arena.c
    src/format.c
    src/numeric.c
    src/string.c
)
target_include_directories(fckit PUBLIC include)
target_compile_features(fckit PUBLIC c_std_11)

if(MSVC)
    target_compile_options(fckit PRIVATE /W4)
else()
    target_compile_options(fckit PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        -ffreestanding -fno-builtin
    )
endif()

if(FCKIT_BUILD_TESTS)
    enable_testing()
    add_executable(fckit_tests tests/test_fckit.c)
    target_link_libraries(fckit_tests PRIVATE fckit)
    target_compile_features(fckit_tests PRIVATE c_std_11)
    if(MSVC)
        target_compile_options(fckit_tests PRIVATE /W4)
    else()
        target_compile_options(fckit_tests PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()
    add_test(NAME fckit_tests COMMAND fckit_tests)
endif()
