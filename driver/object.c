#include <stdint.h>
#include <immintrin.h>

struct object {
    uint64_t __meta[2];
    void *__dat;
    void *__cache;
    void *__next;
    uint16_t __size;
};
