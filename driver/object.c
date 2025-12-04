#include <stdint.h>
#include <immintrin.h>

#define OBJDEF_SIZE ((16 * 2) + 16)

typedef struct __object __object;
typedef __object * tobject;

struct __object
{
    uint64_t __meta[2];
    void    *__dat;
    void    *__cache;
    struct   __object *__next;
    uint16_t __size;
}

 static struct __object * __attribute__((noinline, warn_unused)) __initobject(uint32_t size __attribute_maybe_unused__)
{
    struct __object *object;

    if ((object = malloc(sizeof(struct __object) + OBJDEF_SIZE))) return NULL;
}
