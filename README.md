# Freestanding C Kit

Small freestanding C library I pulled out of FLAP so I could reuse the basic parts in other projects.

It does not need an operating system libc. The host-side tests use libc only to run the test program.

Current features include

- memory and string functions
- signed and unsigned integer parsing
- `snprintf`-style formatting
- low and high arena allocation
- aligned allocations and arena rewind points
- C11 and C++ compatible headers

## Build and test

```text
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Include `fckit/fckit.h` to use the whole library, or include the smaller headers separately.

Released under the BSD 3-Clause License. Please keep the copyright and license notices.
